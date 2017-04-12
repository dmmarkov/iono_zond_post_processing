/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   GpsSatellite.cpp
 * Author: markov
 */

#include "GpsSatellite.hpp"
#include "common_functions.hpp"
#include "DoubleChannelCalculations.hpp"
#include "SvmPredictionData.hpp"

GpsSatellite::GpsSatellite(int satelliteId) : Satellite() {
    satelliteId_ = satelliteId;
    firstChannel_ = nullptr;
    secondChannel_ = nullptr;
    doubleChannelCalculation_ = nullptr;

    satelliteName_ = "GPS" + intToString(satelliteId);
}

GpsSatellite::~GpsSatellite() {
    isEnabledProcessing_ = false;
    destroySatelliteChannels(-90.0, 0, 0);
}

void GpsSatellite::stopThreads() {
    isEnabledProcessing_ = false;
    if ((firstChannel_ != nullptr) && (secondChannel_ != nullptr) && (doubleChannelCalculation_ != nullptr)) {
        ISatelliteChannel* channel = firstChannel_;
        channel->stopThreads();

        channel = secondChannel_;
        channel->stopThreads();

        IDoubleChannelCalculation* dchannel = doubleChannelCalculation_;
        dchannel->stopThreads();
    }
}

void
GpsSatellite::createSatelliteChannels(double elevationAngle, unsigned short week, unsigned int milliseconds) {
    boost::unique_lock<boost::mutex> lock(modifyChannels_);

    if ((elevationAngle > 0) && ((firstChannel_ == nullptr) && (secondChannel_ == nullptr) && (doubleChannelCalculation_ == nullptr))) {
        Satellite::createSatelliteChannels(elevationAngle, week, milliseconds);

        firstChannel_.store(new GpsSatelliteChannel(this, SatelliteChannelType::primary));
        secondChannel_.store(new GpsSatelliteChannel(this, SatelliteChannelType::secondary));
        doubleChannelCalculation_.store(new DoubleChannelCalculations(this));
    }
}

void
GpsSatellite::destroySatelliteChannels(double elevationAngle, unsigned short week, unsigned int milliseconds) {
    boost::unique_lock<boost::mutex> lock(modifyChannels_);
    if ((elevationAngle < 0) && ((firstChannel_ != nullptr) && (secondChannel_ != nullptr) && (doubleChannelCalculation_ != nullptr))) {
        stopThreads();

        SvmPredictionData::getInstance().closeSatellite(this);

        ISatelliteChannel* channel = firstChannel_;
        delete channel;

        channel = secondChannel_;
        delete channel;

        IDoubleChannelCalculation* dchannel = doubleChannelCalculation_;
        delete dchannel;

        firstChannel_ = nullptr;
        secondChannel_ = nullptr;
        doubleChannelCalculation_ = nullptr;

        Satellite::destroySatelliteChannels(elevationAngle, week, milliseconds);
    }
}

SatelliteSystem
GpsSatellite::getSatelliteSystem() {
    return SatelliteSystem::GPS;
}

bool
GpsSatellite::isChannelsExists() {
    boost::unique_lock<boost::mutex> lock(modifyChannels_);
    return (firstChannel_ != nullptr) && (secondChannel_ != nullptr) && (doubleChannelCalculation_ != nullptr);
}