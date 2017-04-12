/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   S4Calculation.cpp
 * Author: markov
 */

#include <thread>
#include <chrono>

#include "S4Calculation.hpp"
#include "gnss_const.hpp"
#include "PlotData.hpp"
#include "SvmPredictionData.hpp"
#include "common_functions.hpp"
#include "ThreadManager.hpp"

S4Calculation::S4Calculation(ISatelliteChannel* satelliteChannel)
: isStopThreads_(false) {
    sumOfDetrendedPower_ = 0;
    sumOfDetrendedPower2_ = 0;

    satelliteChannel_ = satelliteChannel;

    irregulationsSearch_ = new S4IrregulationsSearch(satelliteChannel);

    s4PlotData_ = new PlotData(satelliteChannel_->getSatelliteName(), satelliteChannel_->getChannelName(), "S4 index");

    ThreadManager::getInstance().addNewCalculation(this);

    Logger::getInstance().info("Create S4Calculation for channel " + satelliteChannel_->getFullChannelName());
}

S4Calculation::~S4Calculation() {
//    ThreadManager::getInstance().releaseCalculation(this);

    delete s4PlotData_;

    delete irregulationsSearch_;

    Logger::getInstance().info("Destroy S4Calculation for channel " + satelliteChannel_->getFullChannelName());
}

void
S4Calculation::stopThreads() {
    isStopThreads_ = true;
    ThreadManager::getInstance().releaseCalculation(this);
    irregulationsSearch_->stopThreads();
}

void
S4Calculation::closePlotData() {
    s4PlotData_->disableOutput();
    s4PlotData_->closeFile();
}

void
S4Calculation::pushDetrendedPower(double detrendedPower, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        DetrendedPower el = {week, milliseconds, detrendedPower};
        detrendedPower_.push_back(el);
    }
}

void
S4Calculation::pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) {
    //push the data as much time as needed for 50Hz
    //the normal frequency of this data is 1Hz, so we need to copy it 50 times
    if (!isStopThreads_) {
        for (unsigned int i = 0; i < 1000; i += 20) {
            ElevationAngle el = {week, milliseconds + i, elevationAngle};
            elevationAngle_.push_back(el);
        }
    }
}

void
S4Calculation::pushCarrierToNoiseAndLockTime(double carrierToNoise, double lockTime, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        CarrierToNoiseAndLockTime el = {week, milliseconds, carrierToNoise, lockTime};
        carrierToNoiseAndLockTime_.push_back(el);
    }
}

void
S4Calculation::mergeData() {
    boost::unique_lock<boost::mutex> lockMethod(mergeLock_, boost::try_to_lock);

    if (lockMethod && !isStopThreads_) {
        carrierToNoiseAndLockTime_.lock();
        detrendedPower_.lock();
        elevationAngle_.lock();

        auto it1 = carrierToNoiseAndLockTime_.begin();
        auto it2 = elevationAngle_.begin();
        auto it3 = detrendedPower_.begin();
        bool isReadyForDelete = false;

        auto carrierToNoiseAndLockTimeEnd = carrierToNoiseAndLockTime_.end();
        auto elevationAngleEnd = elevationAngle_.end();
        auto detrendedPowerEnd = detrendedPower_.end();
#ifdef DEBUG
        size_t was_added_values = 0;
#endif
        while ((it1 != carrierToNoiseAndLockTimeEnd) && (it2 != elevationAngleEnd) && (it3 != detrendedPowerEnd)) {
            long int it1Time = (long int) (*it1).week * 604800000 + (long int) (*it1).milliseconds;
            long int it2Time = (long int) (*it2).week * 604800000 + (long int) (*it2).milliseconds;
            long int it3Time = (long int) (*it3).week * 604800000 + (long int) (*it3).milliseconds;

            if (it1Time < it2Time) {
                ++it1;

                continue;
            }

            if (it1Time > it2Time) {
                ++it2;

                continue;
            }

            if (it1Time > it3Time) {
                ++it3;

                continue;
            }

            if (it1Time < it3Time) {
                ++it1;
                ++it2;

                continue;
            }

            if ((it1Time == it2Time) && (it2Time == it3Time)) {
                DataForS4Calculation el = {
                    (*it1).week,
                    (*it1).milliseconds,
                    (*it3).DetrendedPower,
                    (*it1).CarrierToNoise,
                    (*it1).LockTime,
                    (*it2).ElevationAngle,
                    false
                };

                dataForS4Calculation_.push_back(el);

                isReadyForDelete = true;

                ++it1;
                ++it2;
                ++it3;
#ifdef DEBUG
                was_added_values++;
#endif
            } else {
                Logger::getInstance().info("S4 calculation error!!! I'm here but it's wrong!!!");
            }
        }

        if (isReadyForDelete) {
            carrierToNoiseAndLockTime_.erase(carrierToNoiseAndLockTime_.begin(), it1);
            elevationAngle_.erase(elevationAngle_.begin(), it2);
            detrendedPower_.erase(detrendedPower_.begin(), it3);
        } else {
            if (carrierToNoiseAndLockTime_.percentOfFull() > 60) {
                Logger::getInstance().error("S4Calculation. Data inconsistent. carrierToNoiseAndLockTime_ full more than 60%");
                carrierToNoiseAndLockTime_.erase(carrierToNoiseAndLockTime_.begin(), carrierToNoiseAndLockTime_.end());
            }

            if (elevationAngle_.percentOfFull() > 60) {
                Logger::getInstance().error("S4Calculation. Data inconsistent. elevationAngle_ full more than 60%");
                elevationAngle_.erase(elevationAngle_.begin(), elevationAngle_.end());
            }

            if (detrendedPower_.percentOfFull() > 60) {
                Logger::getInstance().error("S4Calculation. Data inconsistent. detrendedPower_ full more than 60%");
                detrendedPower_.erase(detrendedPower_.begin(), detrendedPower_.end());
            }
        }

        carrierToNoiseAndLockTime_.unlock();
        detrendedPower_.unlock();
        elevationAngle_.unlock();
#ifdef DEBUG
        Logger::getInstance().info("S4 calculation was added " + intToString(was_added_values) + " points");
#endif
    }
}

#define DEBUG
void
S4Calculation::calculateData() {
    boost::unique_lock<boost::mutex> lockMethod(calcLock_, boost::try_to_lock);

    if (lockMethod && !isStopThreads_) {

        double waveLength = satelliteChannel_->getWaveLength();
        dataForS4Calculation_.lock();
#ifdef DEBUG
        size_t was_calculated_values = 0;
#endif
        if (dataForS4Calculation_.size() > S4_COUNTER) {
            auto firstNonProcessedElement = dataForS4Calculation_.begin();
            auto firstProcessedElement = dataForS4Calculation_.begin();
            auto lastDataElement = dataForS4Calculation_.end();
            size_t s4Counter = 0;
            while (
                    (firstNonProcessedElement != lastDataElement)
                    &&
                    (*firstNonProcessedElement).isProcessed
                    ) {
                ++firstNonProcessedElement;
                ++s4Counter;
            }

            while (firstNonProcessedElement != lastDataElement) {
                if (((*firstNonProcessedElement).LockTime > S4_MIN_LOCKTIME) && ((*firstNonProcessedElement).ElevationAngle > MIN_ELEVATION_ANGLE)) {
                    if (s4Counter == S4_COUNTER) {
                        //this variant of calculation give the result that is the same as Novatel result
                        //small hack that give advantage of saving memory
                        double oldPower = (*firstProcessedElement).DetrendedPower * 4 * PI / waveLength / waveLength;
                        sumOfDetrendedPower_ -= oldPower;
                        sumOfDetrendedPower2_ -= pow(oldPower, 2);

                        ++firstProcessedElement;

                        double newPower = (*firstNonProcessedElement).DetrendedPower * 4 * PI / waveLength / waveLength;
                        sumOfDetrendedPower_ += newPower;
                        sumOfDetrendedPower2_ += pow(newPower, 2);

                        double sum_p = sumOfDetrendedPower_ / S4_COUNTER;
                        double sum_p_2 = sum_p * sum_p;
                        double sum_p2 = sumOfDetrendedPower2_ / S4_COUNTER;

                        double carrierToNoiseBase = pow(10.0, (*firstNonProcessedElement).CarrierToNoise / 10.0);
                        double s4Noise = 100.0 / carrierToNoiseBase * (1.0 + 500.0 / 19.0 / carrierToNoiseBase);

                        double s4Value = pow((sum_p2 - sum_p_2) / sum_p_2, 0.5);
                        double s4WithoutNoiseValue = pow((sum_p2 - sum_p_2) / sum_p_2 - s4Noise, 0.5);


                        SvmPredictionData::getInstance().pushS4Value(satelliteChannel_->getSatellite(),
                                                                     satelliteChannel_, s4Value,
                                                                     (*firstNonProcessedElement).week,
                                                                     (*firstNonProcessedElement).milliseconds);

                        SvmPredictionData::getInstance().pushS4WithoutNoiseValue(satelliteChannel_->getSatellite(),
                                                                                 satelliteChannel_, s4WithoutNoiseValue,
                                                                                 (*firstNonProcessedElement).week,
                                                                                 (*firstNonProcessedElement).milliseconds);

                        if ((*firstNonProcessedElement).ElevationAngle > 30.0) {
                            irregulationsSearch_->pushS4Value((*firstNonProcessedElement).week,
                                                              (*firstNonProcessedElement).milliseconds,
                                                              s4Value);
                        }
#ifdef DEBUG
                        was_calculated_values++;
#endif
                    } else {
                        double newPower = (*firstNonProcessedElement).DetrendedPower * 4 * PI / waveLength / waveLength;

                        sumOfDetrendedPower_ += newPower;
                        sumOfDetrendedPower2_ += pow(newPower, 2);

                        s4Counter++;

                        if (s4Counter == S4_COUNTER) {
                            double sum_p = sumOfDetrendedPower_ / S4_COUNTER;
                            double sum_p_2 = sum_p * sum_p;
                            double sum_p2 = sumOfDetrendedPower2_ / S4_COUNTER;

                            double s4Value = pow((sum_p2 - sum_p_2) / sum_p_2 /*- carrierToNoiseS4_2*/, 0.5);

                            SvmPredictionData::getInstance().pushS4Value(satelliteChannel_->getSatellite(),
                                             satelliteChannel_, s4Value,
                                             (*firstNonProcessedElement).week,
                                             (*firstNonProcessedElement).milliseconds);
                        }
                    }

                    (*firstNonProcessedElement).isProcessed = true;
                    ++firstNonProcessedElement;
                } else {
                    //lost tracking loop
                    sumOfDetrendedPower_ = 0;
                    sumOfDetrendedPower2_ = 0;

                    //it's need for erasing useless data after the processing
                    ++firstNonProcessedElement;
                    firstProcessedElement = firstNonProcessedElement;

                    s4Counter = 0;
                }
            }

            //erase first elements that was processed and don't need in the future
            if (firstProcessedElement != dataForS4Calculation_.begin()) {
                dataForS4Calculation_.erase(dataForS4Calculation_.begin(), firstProcessedElement);
            }
#ifdef DEBUG
            Logger::getInstance().info("S4 calculation was calculated " + intToString(was_calculated_values) + " points");
#endif
        }

        dataForS4Calculation_.unlock();
    }
}

void
S4Calculation::forceCloseOutDataFiles(GnssLogTime& time) {
    s4PlotData_->forceCloseOutDataFiles(time);
}