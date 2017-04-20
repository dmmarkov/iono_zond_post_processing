/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   DoubleChannelCalculations.cpp
 * Author: dmitri
 */

#include <thread>
#include <chrono>

#include "DoubleChannelCalculations.hpp"

#include "GnssLogTime.hpp"
#include "struct_headers.hpp"
#include "common_functions.hpp"
#include "GlonassSatellite.hpp"
#include "gnss_const.hpp"
#include "SvmPredictionData.hpp"
#include "ThreadManager.hpp"

DoubleChannelCalculations::DoubleChannelCalculations(ISatellite* satellite) :
isStopThreads_(false) {
    parentSatellite_ = satellite;

    switch (parentSatellite_->getSatelliteSystem()) {
        case SatelliteSystem::GPS:
        {
            frequency1_ = GPS_L1_FREQUENCY;
            waveLength1_ = GPS_L1_WAVE;

            frequency2_ = GPS_L2_FREQUENCY;
            waveLength2_ = GPS_L2_WAVE;

            glonassFrequency_ = 0;
            break;
        }
        case SatelliteSystem::GLONASS:
        {
            GlonassSatellite* glonassSatellite = dynamic_cast<GlonassSatellite*> (satellite);
            frequency1_ = GLO_L1_BASE_FREQUECY + GLO_L1_DELTA_FREQUECY * glonassSatellite->getGlonassFrequency();
            waveLength1_ = SPEED_OF_LIGHT / frequency1_;

            frequency2_ = GLO_L2_BASE_FREQUECY + GLO_L2_DELTA_FREQUECY * glonassSatellite->getGlonassFrequency();
            waveLength2_ = SPEED_OF_LIGHT / frequency2_;

            glonassFrequency_ = glonassSatellite->getGlonassFrequency();
            break;
        }
        default:
        {
            assert(false);
        }
    }

    tecValuesPlotData_ = new PlotData(parentSatellite_->getSatelliteName(), "TecByPhaseWithoutAmbiguity");
    tecValuesPlotData_->disableOutput();

    long double L1_L2_RESIDUAL = frequency1_ * frequency1_ - frequency2_ * frequency2_;
    tecCoefficient_ = TEC_G * L1_L2_RESIDUAL / frequency1_ / frequency1_ / frequency2_ / frequency2_ * TECU_KOEFFICIENT;

    pashintsevCalculation_ = new pashintsev::PashintsevCalculation(satellite);

    ThreadManager::getInstance().addNewCalculation(this);

    Logger::getInstance().info("Create DoubleChannelCalculations for satellite " + parentSatellite_->getSatelliteName());
}

DoubleChannelCalculations::~DoubleChannelCalculations() {
//    ThreadManager::getInstance().releaseCalculation(this);

    delete pashintsevCalculation_;
    delete tecValuesPlotData_;

    Logger::getInstance().info("Destroy DoubleChannelCalculations for satellite " + parentSatellite_->getSatelliteName());
}

void
DoubleChannelCalculations::stopThreads() {
    isStopThreads_ = true;

    ThreadManager::getInstance().releaseCalculation(this);

    pashintsevCalculation_->stopThreads();
}

void
DoubleChannelCalculations::pushPsrPhaseLockTime1(double pseudorange, double carrierPhase, double lockTime, double carrierToNoise, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        ChannelData el = {
            .week = week,
            .milliseconds = milliseconds,
            .pseudorange = pseudorange,
            .carrierPhase = carrierPhase,
            .lockTime = lockTime,
            .carrierToNoise = carrierToNoise
        };

        firstChannel_.push_back(el);
    }
}

void
DoubleChannelCalculations::pushPsrPhaseLockTime2(double pseudorange, double carrierPhase, double lockTime, double carrierToNoise, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        ChannelData el = {
            .week = week,
            .milliseconds = milliseconds,
            .pseudorange = pseudorange,
            .carrierPhase = carrierPhase,
            .lockTime = lockTime,
            .carrierToNoise = carrierToNoise
        };

        secondChannel_.push_back(el);
    }
}

void
DoubleChannelCalculations::pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        for (unsigned int i = 0; i < 1000; i += 20) {
            ElevationAngle el = {
                .week = week,
                .milliseconds = milliseconds + i,
                .elevationAngle = elevationAngle
            };

            elevationAngle_.push_back(el);
        }
    }
}

void
DoubleChannelCalculations::mergeData() {
    boost::unique_lock<boost::mutex> lockMethod(mergeLock_, boost::try_to_lock);

    if (lockMethod && !isStopThreads_) {
    //merge the data from different sources
        firstChannel_.lock();
        secondChannel_.lock();
        elevationAngle_.lock();

        auto it1 = firstChannel_.begin();
        auto it2 = secondChannel_.begin();
        auto it3 = elevationAngle_.begin();

        auto firstChannelEnd = firstChannel_.end();
        auto secondChannelEnd = secondChannel_.end();
        auto elevationAngleEnd = elevationAngle_.end();

        bool isReadyForDelete = false;
#ifdef DEBUG
        size_t was_merged = 0;
#endif

        while ((it1 != firstChannelEnd) && (it2 != secondChannelEnd) && (it3 != elevationAngleEnd)) {
            long int it1Time = getMillisecondsTime((*it1).week, (*it1).milliseconds);
            long int it2Time = getMillisecondsTime((*it2).week, (*it2).milliseconds);
            long int it3Time = getMillisecondsTime((*it3).week, (*it3).milliseconds);

            if (it1Time < it2Time) {
                ++it1;
                continue;
            }

            if (it2Time < it1Time) {
                ++it2;
                continue;
            }

            if (it1Time > it3Time) {
                ++it3;
                continue;
            }

            if (it3Time > it1Time) {
                ++it1;
                ++it2;
                continue;
            }


            DataForDoubleChannelCalculation el = {
                .week = (*it1).week,
                .milliseconds = (*it1).milliseconds,
                .pseudorange1 = (*it1).pseudorange,
                .carrierPhase1 = (*it1).carrierPhase,
                .lockTime1 = (*it1).lockTime,
                .carrierToNoise1 = (*it1).carrierToNoise,
                .pseudorange2 = (*it2).pseudorange,
                .carrierPhase2 = (*it2).carrierPhase,
                .lockTime2 = (*it2).lockTime,
                .carrierToNoise2 = (*it2).carrierToNoise,
                .elevationAngle = (*it3).elevationAngle,
                .isProcessed = false
            };

            dataForCalculation_.push_back(el);
#ifdef DEBUG
            was_merged++;
#endif
            isReadyForDelete = true;

            ++it1;
            ++it2;
            ++it3;
        }
#ifdef DEBUG
        Logger::getInstance().info("DoubleChannelCalculations::mergeDataForCalculation() was_merged = " + intToString(was_merged) + " points");
#endif

        if (isReadyForDelete) {
            firstChannel_.erase(firstChannel_.begin(), it1);
            secondChannel_.erase(secondChannel_.begin(), it2);
            elevationAngle_.erase(elevationAngle_.begin(), it3);
        } else {
            if (firstChannel_.percentOfFull() > 60) {
                Logger::getInstance().error("DoubleChannelCalculations. Channel inconsistent. firstChannel_ full more than 60%");
                firstChannel_.erase(firstChannel_.begin(), firstChannel_.end());
            }

            if (secondChannel_.percentOfFull() > 60) {
                Logger::getInstance().error("DoubleChannelCalculations. Channel inconsistent. secondChannel_ full more than 60%");
                secondChannel_.erase(secondChannel_.begin(), secondChannel_.end());
            }

            if (elevationAngle_.percentOfFull() > 60) {
                Logger::getInstance().error("DoubleChannelCalculations. Channel inconsistent. elevationAngle_ full more than 60%");
                elevationAngle_.erase(elevationAngle_.begin(), elevationAngle_.end());
            }
        }

        elevationAngle_.unlock();
        firstChannel_.unlock();
        secondChannel_.unlock();
    }
}

void
DoubleChannelCalculations::calculateData() {
    boost::unique_lock<boost::mutex> lockMethod(calcLock_, boost::try_to_lock);

    if (lockMethod && !isStopThreads_) {
        dataForCalculation_.lock();
#ifdef DEBUG
        size_t was_calculated = 0;
#endif
        for (auto it = dataForCalculation_.begin(); it != dataForCalculation_.end(); ++it) {
            if (((*it).lockTime1 > DOUBLE_CHANNEL_MIN_LOCKTIME) && ((*it).lockTime2 > DOUBLE_CHANNEL_MIN_LOCKTIME) && ((*it).elevationAngle > MIN_ELEVATION_ANGLE)) {

                double l1 = (*it).carrierPhase1 * waveLength1_;
                double l2 = (*it).carrierPhase2 * waveLength2_;
                double tec = (l1 - l2) / tecCoefficient_;

                pashintsevCalculation_->pushTecWithoutAmbiguity(tec, (*it).week, (*it).milliseconds);

                SvmPredictionData::getInstance().pushValuesForTec(parentSatellite_,
                                                                  (*it).pseudorange1,
                                                                  (*it).pseudorange2,
                                                                  (*it).carrierPhase1,
                                                                  (*it).carrierPhase2,
                                                                  (*it).carrierToNoise1,
                                                                  (*it).carrierToNoise2,
                                                                  frequency1_,
                                                                  frequency2_,
                                                                  waveLength1_,
                                                                  waveLength2_,
                                                                  (*it).week,
                                                                  (*it).milliseconds,
                                                                  tec);
#ifdef DEBUG
                was_calculated++;
#endif
            }
        }
#ifdef DEBUG
        Logger::getInstance().info("DoubleChannelCalculations::calculateData() was_calculated = " + intToString(was_calculated) + " points");
#endif
        if (!dataForCalculation_.is_empty()) {
            dataForCalculation_.erase(dataForCalculation_.begin(), dataForCalculation_.end());
        }

        dataForCalculation_.unlock();
    }
}

void
DoubleChannelCalculations::closePlotData() {
    pashintsevCalculation_->closePlotData();

    tecValuesPlotData_->disableOutput();
    tecValuesPlotData_->closeFile();
}

void
DoubleChannelCalculations::forceCloseOutDataFiles(GnssLogTime& time) {
    pashintsevCalculation_->forceCloseOutDataFiles(time);

    tecValuesPlotData_->forceCloseOutDataFiles(time);
}