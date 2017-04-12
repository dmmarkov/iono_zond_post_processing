/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SvmPredictionData.cpp
 * Author: dmitri
 *
 * Created on 13 Июнь 2016 г., 17:24
 */

#include <list>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <mutex>

#include "SvmPredictionData.hpp"
#include "gnss_const.hpp"
#include "gnss_functions.hpp"
#include "common_functions.hpp"
#include "server_consts.hpp"
#include "GlonassSatellite.hpp"
#include "PredictionsExperiments.hpp"
#include "Logger.hpp"

SvmPredictionData::SvmPredictionData() : isActive_(false) {
    isExperiment0 = false;
    isExperiment1 = false;
    isExperiment2 = false;
    isExperiment3 = false;
    isExperiment4 = false;
    isExperiment5 = false;
    isExperiment6 = false;
    isExperiment7 = false;
    isExperiment8 = false;
    isExperiment9 = false;
    isExperiment10 = false;
    isExperiment11 = false;
    isExperiment12 = false;
    isExperiment13 = false;
    isExperiment14 = false;
    isExperiment15 = false;
    isExperiment16 = false;
    isExperiment17 = false;
    isExperiment18 = false;

    numberOfExperiment_ = -1;

    filterSatelliteSystem = -1;
    filterSatelliteId = -1;
    filterSatelliteChannel = -1;

}

SvmPredictionData::~SvmPredictionData() {
}

void SvmPredictionData::restart() {
    stop();
    isActive_ = true;
}

void
SvmPredictionData::start(std::string& nameOfExperiment, std::string& nameOfSatelliteSystem, std::string& idOfSatellite, std::string& nameOfSatelliteChannel) {
    isActive_ = true;

    if (nameOfExperiment == "experiment0") {
        isExperiment0 = true;
        numberOfExperiment_ = 0;
    }

    if (nameOfExperiment == "experiment1") {
        isExperiment1 = true;
        numberOfExperiment_ = 1;
    }

    if (nameOfExperiment == "experiment2") {
        isExperiment2 = true;
        numberOfExperiment_ = 2;
    }

    if (nameOfExperiment == "experiment3") {
        isExperiment3 = true;
        numberOfExperiment_ = 3;
    }

    if (nameOfExperiment == "experiment4") {
        isExperiment4 = true;
        numberOfExperiment_ = 4;
    }

    if (nameOfExperiment == "experiment5") {
        isExperiment5 = true;
        numberOfExperiment_ = 5;
    }

    if (nameOfExperiment == "experiment6") {
        isExperiment6 = true;
        numberOfExperiment_ = 6;
    }

    if (nameOfExperiment == "experiment7") {
        isExperiment7 = true;
        numberOfExperiment_ = 7;
    }

    if (nameOfExperiment == "experiment8") {
        isExperiment8 = true;
        numberOfExperiment_ = 8;
    }

    if (nameOfExperiment == "experiment9") {
        isExperiment9 = true;
        numberOfExperiment_ = 9;
    }

    if (nameOfExperiment == "experiment10") {
        isExperiment10 = true;
        numberOfExperiment_ = 10;
    }

    if (nameOfExperiment == "experiment11") {
        isExperiment11 = true;
        numberOfExperiment_ = 11;
    }

    if (nameOfExperiment == "experiment12") {
        isExperiment12 = true;
        numberOfExperiment_ = 12;
    }

    if (nameOfExperiment == "experiment13") {
        isExperiment13 = true;
        numberOfExperiment_ = 13;
    }

    if (nameOfExperiment == "experiment14") {
        isExperiment14 = true;
        numberOfExperiment_ = 14;
    }

    if (nameOfExperiment == "experiment15") {
        isExperiment15 = true;
        numberOfExperiment_ = 15;
    }

    if (nameOfExperiment == "experiment16") {
        isExperiment16 = true;
        numberOfExperiment_ = 16;
    }

    if (nameOfExperiment == "experiment17") {
        isExperiment17 = true;
        numberOfExperiment_ = 17;
    }

    if (nameOfExperiment == "experiment18") {
        isExperiment18 = true;
        numberOfExperiment_ = 18;
    }

    if (nameOfSatelliteSystem != "") {
        if (nameOfSatelliteSystem == "GPS") {
            filterSatelliteSystem = 0;
        }

        if (nameOfSatelliteSystem == "GLONASS") {
            filterSatelliteSystem = 1;
        }

        if (idOfSatellite != "") {
            filterSatelliteId = std::stoi(idOfSatellite);
        }

        if (nameOfSatelliteChannel == "L1") {
            filterSatelliteChannel = 1;
        }

        if (nameOfSatelliteChannel == "L2") {
            filterSatelliteChannel = 2;
        }
    }
}

void
SvmPredictionData::stop() {
    isActive_ = false;

    Logger::getInstance().info("SvmPredictionData::stop() isActive_ = false");

    std::list<ISatellite*> satellites;

    {
        // get upgradable access
        boost::upgrade_lock<boost::shared_mutex> lock(mutexLockSatelliteAndChannels_);
        // get exclusive access
        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

        for (auto& el : satellites_) {
            el.second->stopThreads();
            satellites.push_back(el.second->satellite_);
        }
    }

    for (auto it = satellites.begin(); it != satellites.end(); ++it) {
        closeSatellite(*it);
    }

    Logger::getInstance().info("End of SvmPredictionData::stop()");
}

int
SvmPredictionData::getKeyId(ISatellite* satellite) {

    switch (satellite->getSatelliteSystem()) {
        case SatelliteSystem::GPS:
            return 000 + satellite->getSatelliteId();
        case SatelliteSystem::GLONASS:
            return 100 + satellite->getSatelliteId();
        default:
            return 200 + satellite->getSatelliteId();
    }
}

void
SvmPredictionData::addSatellite(ISatellite* satellite,
                                unsigned short week,
                                unsigned int milliseconds) {
    if (!isActive_) {
        return;
    }

    // get upgradable access
    boost::upgrade_lock<boost::shared_mutex> lock(mutexLockSatelliteAndChannels_);
    // get exclusive access
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

    PredictionsExperiments *key = new PredictionsExperiments(satellite, week, milliseconds, numberOfExperiment_, filterSatelliteSystem, filterSatelliteId, filterSatelliteChannel);
    satellites_.insert(std::make_pair(getKeyId(satellite), key));
}

void
SvmPredictionData::closeSatellite(ISatellite* satellite) {

    // get upgradable access
    boost::upgrade_lock<boost::shared_mutex> lock(mutexLockSatelliteAndChannels_);
    // get exclusive access
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

    int key = getKeyId(satellite);
    try {
        auto experiment = satellites_.at(key);
        experiment->stopThreads();
        experiment->closePlotData();
        delete experiment;
        satellites_.erase(key);
    } catch (std::out_of_range& ex) {
        Logger::getInstance().error("Satellite was not found " + satellite->getSatelliteName());
    }
}

void
SvmPredictionData::pushS4Value(ISatellite* satellite,
                               ISatelliteChannel* channel,
                               double s4,
                               unsigned short week,
                               unsigned int milliseconds) {
    if (!isActive_) {
        return;
    }

    boost::shared_lock<boost::shared_mutex> lock(mutexLockSatelliteAndChannels_);

    auto exp = satellites_.at(getKeyId(satellite));
    if (channel->getChannelName() == "L1") {
        exp->pushS4ValuesL1(s4, week, milliseconds);

        if (satellite->getSatelliteSystem() == SatelliteSystem::GPS) {
            //докинуть в L2 просто нолик
            exp->pushS4ValuesL2(0, week, milliseconds);
        }
    }

    if (channel->getChannelName() == "L2") {
        exp->pushS4ValuesL2(s4, week, milliseconds);
    }
}

void
SvmPredictionData::pushS4WithoutNoiseValue(ISatellite* satellite,
                                           ISatelliteChannel* channel,
                                           double s4,
                                           unsigned short week,
                                           unsigned int milliseconds) {
    if (!isActive_) {
        return;
    }

    boost::shared_lock<boost::shared_mutex> lock(mutexLockSatelliteAndChannels_);

    auto exp = satellites_.at(getKeyId(satellite));
    if (channel->getChannelName() == "L1") {
        exp->pushS4WithoutNoiseValuesL1(s4, week, milliseconds);

        if (satellite->getSatelliteSystem() == SatelliteSystem::GPS) {
            //докинуть в L2 просто нолик
            exp->pushS4WithoutNoiseValuesL2(0, week, milliseconds);
        }
    }

    if (channel->getChannelName() == "L2") {
        exp->pushS4WithoutNoiseValuesL2(s4, week, milliseconds);
    }
}

void
SvmPredictionData::pushSigmaPhiValue(ISatellite* satellite,
                                     ISatelliteChannel* channel,
                                     double sigmaPhi,
                                     unsigned short week,
                                     unsigned int milliseconds) {
    if (!isActive_) {
        return;
    }

    boost::shared_lock<boost::shared_mutex> lock(mutexLockSatelliteAndChannels_);

    auto exp = satellites_.at(getKeyId(satellite));
    if (channel->getChannelName() == "L1") {
        exp->pushSigmaPhiValuesL1(sigmaPhi, week, milliseconds);

        if (satellite->getSatelliteSystem() == SatelliteSystem::GPS) {
            //докинуть в L2 просто нолик
            exp->pushSigmaPhiValuesL2(0, week, milliseconds);
        }
    }

    if (channel->getChannelName() == "L2") {
        exp->pushSigmaPhiValuesL2(sigmaPhi, week, milliseconds);
    }
}

void
SvmPredictionData::pushElevationAngle(ISatellite* satellite,
                                      double elevationAngle,
                                      unsigned short week,
                                      unsigned int milliseconds) {
    if (!isActive_) {
        return;
    }

    boost::shared_lock<boost::shared_mutex> lock(mutexLockSatelliteAndChannels_);

    auto exp = satellites_.at(getKeyId(satellite));
    exp->pushElevationAngle(elevationAngle, week, milliseconds);
}

void
SvmPredictionData::pushValuesForTec(ISatellite* satellite,
                                    double PsrML1,
                                    double PsrML2,
                                    double PsrFL1,
                                    double PsrFL2,
                                    double carrierToNoiseL1,
                                    double carrierToNoiseL2,
                                    double frequencyL1,
                                    double frequencyL2,
                                    double waveLengthL1,
                                    double waveLengthL2,
                                    unsigned short week,
                                    unsigned int milliseconds,
                                    double tec) {
    if (!isActive_) {
        return;
    }

    boost::shared_lock<boost::shared_mutex> lock(mutexLockSatelliteAndChannels_);

    auto exp = satellites_.at(getKeyId(satellite));
    exp->pushValuesForTec(PsrML1, PsrML2,
                          PsrFL1, PsrFL2,
                          carrierToNoiseL1, carrierToNoiseL2,
                          frequencyL1, frequencyL2,
                          waveLengthL1, waveLengthL2,
                          tec,
                          week, milliseconds);
}