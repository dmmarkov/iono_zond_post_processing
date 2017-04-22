/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SigmaPhiCalculation.cpp
 * Author: markov
 */

#include <thread>
#include <chrono>

#include "SigmaPhiCalculation.hpp"
#include "gnss_const.hpp"
#include "SvmPredictionData.hpp"
#include "common_functions.hpp"
#include "ThreadManager.hpp"

SigmaPhiCalculation::SigmaPhiCalculation(ISatelliteChannel* satelliteChannel) :
        isStopThreads_(false)
{
    satelliteChannel_ = satelliteChannel;

    sumOfDetrendedAdr_ = 0;
    sumOfDetrendedAdr2_ = 0;

    sigmaPhiPlotData_ = new PlotData(satelliteChannel_->getSatelliteName(), satelliteChannel_->getChannelName(), "SigmaPhi index");

    ThreadManager::getInstance().addNewCalculation(this);

    Logger::getInstance().info("Create SigmaPhiCalculation for channel " + satelliteChannel_->getFullChannelName());
}

SigmaPhiCalculation::~SigmaPhiCalculation() {
//    ThreadManager::getInstance().releaseCalculation(this);

    delete sigmaPhiPlotData_;

    Logger::getInstance().info("Destroy SigmaPhiCalculation for channel " + satelliteChannel_->getFullChannelName());
}

void
SigmaPhiCalculation::stopThreads() {
    isStopThreads_ = true;
    ThreadManager::getInstance().releaseCalculation(this);
}

void
SigmaPhiCalculation::closePlotData() {
    sigmaPhiPlotData_->disableOutput();
    sigmaPhiPlotData_->closeFile();
}

void
SigmaPhiCalculation::pushDetrendedAdr(double detrendedAdr, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        DetrendedAdr el = {
            .week = week,
            .milliseconds = milliseconds,
            .DetrendedAdr = detrendedAdr
        };

        detrendedAdr_.push_back(el);
    }
}

void
SigmaPhiCalculation::pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        for (unsigned int i = 0; i < 1000; i += 20) {
            ElevationAngle el = {
                .week = week,
                .milliseconds = milliseconds + i,
                .ElevationAngle = elevationAngle
            };

            elevationAngle_.push_back(el);
        }
    }
}

void
SigmaPhiCalculation::pushLockTime(double lockTime, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        LockTime el = {
            .week = week,
            .milliseconds = milliseconds,
            .LockTime = lockTime
        };

        lockTime_.push_back(el);
    }
}

void
SigmaPhiCalculation::mergeData() {
    boost::unique_lock<boost::mutex> lockMethod(mergeLock_, boost::try_to_lock);

    if (lockMethod && !isStopThreads_) {
        lockTime_.lock();
        detrendedAdr_.lock();
        elevationAngle_.lock();

        auto it1 = lockTime_.begin();
        auto it2 = elevationAngle_.begin();
        auto it3 = detrendedAdr_.begin();

        bool isReadyForDelete = false;

        auto lockTimeEnd = lockTime_.end();
        auto elevationAngleEnd = elevationAngle_.end();
        auto detrendedAdrEnd = detrendedAdr_.end();

        while ((it1 != lockTimeEnd) && (it2 != elevationAngleEnd) && (it3 != detrendedAdrEnd)) {
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
                DataForSigmaPhiCalculation el = {
                    .week = (*it1).week,
                    .milliseconds = (*it1).milliseconds,
                    .DetrendedAdr = (*it3).DetrendedAdr,
                    .LockTime = (*it1).LockTime,
                    .ElevationAngle = (*it2).ElevationAngle,
                    .isProcessed = false
                };

                dataForSigmaPhiCalculation_.push_back(el);

                isReadyForDelete = true;

                ++it1;
                ++it2;
                ++it3;
            } else {
                Logger::getInstance().error("SigmaPhiCalculation::mergeDataForSigmaPhiCalculation() Impossible!!!! I shouldn't be here!");
            }
        }

        if (isReadyForDelete) {
            lockTime_.erase(lockTime_.begin(), it1);
            elevationAngle_.erase(elevationAngle_.begin(), it2);
            detrendedAdr_.erase(detrendedAdr_.begin(), it3);
        } else {
            if (lockTime_.percentOfFull() > 60) {
                Logger::getInstance().error("SigmaPhiCalculation. Data inconsistent. lockTime_ full more than 60%");
                lockTime_.erase(lockTime_.begin(), lockTime_.end());
            }

            if (elevationAngle_.percentOfFull() > 60) {
                Logger::getInstance().error("SigmaPhiCalculation. Data inconsistent. elevationAngle_ full more than 60%");
                elevationAngle_.erase(elevationAngle_.begin(), elevationAngle_.end());
            }

            if (detrendedAdr_.percentOfFull() > 60) {
                Logger::getInstance().error("SigmaPhiCalculation. Data inconsistent. detrendedAdr_ full more than 60%");
                detrendedAdr_.erase(detrendedAdr_.begin(), detrendedAdr_.end());
            }
        }

        lockTime_.unlock();
        detrendedAdr_.unlock();
        elevationAngle_.unlock();
    }
}

void
SigmaPhiCalculation::calculateData() {
    boost::unique_lock<boost::mutex> lockMethod(calcLock_, boost::try_to_lock);

    if (lockMethod && !isStopThreads_) {
        dataForSigmaPhiCalculation_.lock();

        if (dataForSigmaPhiCalculation_.size() > SIGMA_PHI_COUNTER) {
            try {
                //find first that was not processed
                auto firstNonProcessedElement = dataForSigmaPhiCalculation_.begin();
                auto firstProcessedElement = dataForSigmaPhiCalculation_.begin();
                auto dataEnd = dataForSigmaPhiCalculation_.end();
                size_t sigmaPhiCounter = 0;
                while (
                        (firstNonProcessedElement != dataEnd)
                        &&
                        (*firstNonProcessedElement).isProcessed
                        ) {
                    ++firstNonProcessedElement;
                    ++sigmaPhiCounter;
                }
#ifdef DEBUG
                size_t was_calculated = 0;
#endif
                while (firstNonProcessedElement != dataEnd) {
                    if (((*firstNonProcessedElement).LockTime > SIGMA_PHI_MIN_LOCKTIME) && ((*firstNonProcessedElement).ElevationAngle > MIN_ELEVATION_ANGLE)) {
                        if (sigmaPhiCounter == SIGMA_PHI_COUNTER) {
                            //small hack that give advantage of saving memory
                            double oldPhase = (*firstProcessedElement).DetrendedAdr * PI * 2;
                            sumOfDetrendedAdr_ -= oldPhase;
                            sumOfDetrendedAdr2_ -= pow(oldPhase, 2);

                            ++firstProcessedElement;

                            double newPhase = (*firstNonProcessedElement).DetrendedAdr * PI * 2;
                            sumOfDetrendedAdr_ += newPhase;
                            sumOfDetrendedAdr2_ += pow(newPhase, 2);

                            double sum_f = sumOfDetrendedAdr_ / SIGMA_PHI_COUNTER;
                            sum_f *= sum_f;
                            double sum_f2 = sumOfDetrendedAdr2_ / SIGMA_PHI_COUNTER;

                            double newSigmaPhi = pow(sum_f2 - sum_f, 0.5);

                            SvmPredictionData::getInstance().pushSigmaPhiValue(satelliteChannel_->getSatellite(),
                                                                     satelliteChannel_, newSigmaPhi,
                                                                     (*firstNonProcessedElement).week,
                                                                     (*firstNonProcessedElement).milliseconds);
#ifdef DEBUG
                            was_calculated++;
#endif
                        } else {
                            double newPhase = (*firstNonProcessedElement).DetrendedAdr * PI * 2;
                            sumOfDetrendedAdr_ += newPhase;
                            sumOfDetrendedAdr2_ += pow(newPhase, 2);

                            sigmaPhiCounter++;
                        }

                        (*firstNonProcessedElement).isProcessed = true;
                        ++firstNonProcessedElement;
                    } else {
                        sumOfDetrendedAdr_ = 0;
                        sumOfDetrendedAdr2_ = 0;

                        //it's need for erasing useless data after the processing
                        ++firstNonProcessedElement;
                        firstProcessedElement = firstNonProcessedElement;

                        sigmaPhiCounter = 0;
                    }

                }
#ifdef DEBUG
                Logger::getInstance().info("calculateSigmaPhiData was_calculated = " + intToString(was_calculated) + " points");
#endif
                if (firstProcessedElement != dataForSigmaPhiCalculation_.begin()) {
                    dataForSigmaPhiCalculation_.erase(dataForSigmaPhiCalculation_.begin(), firstProcessedElement);
                }
            }
            catch (std::exception& ex) {
                Logger::getInstance().error(std::string("SigmaPhiCalculation::calculateSigmaPhiData() error: ") +ex.what());
            }
        }

        dataForSigmaPhiCalculation_.unlock();
    }
}

void
SigmaPhiCalculation::forceCloseOutDataFiles(GnssLogTime& time) {
    sigmaPhiPlotData_->forceCloseOutDataFiles(time);
}