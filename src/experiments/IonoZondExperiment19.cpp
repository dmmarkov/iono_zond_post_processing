/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   IonoZondExperiment19.cpp
 * Author: dmitri
 *
 * Created on 22 апреля 2017 г., 16:31
 */

#include <sstream>

#include "IonoZondExperiment19.hpp"
#include "Logger.hpp"
#include "common_functions.hpp"

IonoZondExperiment19::IonoZondExperiment19(ISatellite* satellite, std::string channelName) {
    satellite_ = satellite;
    channelName_ = channelName;

    minSigmaPhiValue_ = 0.0;
    maxSigmaPhiValue_ = 1.0;
    intervalCount_ = 80;
    stepOfCounter_ = (maxSigmaPhiValue_ - minSigmaPhiValue_) / (double) intervalCount_;

    counters_.resize(intervalCount_);
}

IonoZondExperiment19::~IonoZondExperiment19() {

    std::stringstream str;

    switch (satellite_->getSatelliteSystem()) {
        case SatelliteSystem::GPS:
            str << "GPS ";
            break;
        case SatelliteSystem::GLONASS:
            str << "GLONASS ";
            break;
        default:
            Logger::getInstance().error("Unknown satellite system");
            break;
    }

    str << satellite_->getSatelliteId() << " " << channelName_;

    for (auto &t : counters_) {
        str << " " << t;
    }

    printCout(str.rdbuf());
}

void
IonoZondExperiment19::pushSigmaPhi(const double sigmaPhi) {
    size_t i = (size_t) (sigmaPhi / stepOfCounter_);
    counters_.at(i)++;
}