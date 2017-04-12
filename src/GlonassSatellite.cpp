/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   GlonassSatellite.cpp
 * Author: markov
 */

#include "GlonassSatellite.hpp"

#include "gnss_const.hpp"
#include "common_functions.hpp"
#include "DoubleChannelCalculations.hpp"
#include "SvmPredictionData.hpp"

GlonassSatellite::GlonassSatellite(int satelliteId, char frequency) : Satellite() {
    satelliteId_ = satelliteId;
    frequency_ = frequency;
    firstChannel_ = nullptr;
    secondChannel_ = nullptr;
    doubleChannelCalculation_ = nullptr;

    satelliteName_ = "GLONASS" + intToString(satelliteId + GLONASS_SATELLITE_RANGE_CONST);
}

GlonassSatellite::~GlonassSatellite() {
    isEnabledProcessing_ = false;
    destroySatelliteChannels(-90.0, 0, 0);
}

void
GlonassSatellite::stopThreads() {
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
GlonassSatellite::createSatelliteChannels(double elevationAngle, unsigned short week, unsigned int milliseconds) {
    boost::unique_lock<boost::mutex> lock(modifyChannels_);

    if ((elevationAngle > 0) && ((firstChannel_ == nullptr) && (secondChannel_ == nullptr) && (doubleChannelCalculation_ == nullptr))) {
        firstChannel_ = new GlonassSatelliteChannel(this, SatelliteChannelType::primary, frequency_);
        secondChannel_ = new GlonassSatelliteChannel(this, SatelliteChannelType::secondary, frequency_);
        doubleChannelCalculation_ = new DoubleChannelCalculations(this);

        Satellite::createSatelliteChannels(elevationAngle, week, milliseconds);
    }
}

void
GlonassSatellite::destroySatelliteChannels(double elevationAngle, unsigned short week, unsigned int milliseconds) {
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

char
GlonassSatellite::getGlonassFrequency(char frequency, bool isRangeSource) {
    char glonassFrequency = frequency;

    if (isRangeSource) {
        glonassFrequency -= GLONASS_FREQUENCY_RANGE_CONST;
    }

    return glonassFrequency;
}

char
GlonassSatellite::getGlonassFrequency() {
    return frequency_;
}

SatelliteSystem
GlonassSatellite::getSatelliteSystem() {
    return SatelliteSystem::GLONASS;
}

bool
GlonassSatellite::isChannelsExists() {
    boost::unique_lock<boost::mutex> lock(modifyChannels_);
    return ((firstChannel_ != nullptr) && (secondChannel_ != nullptr) && (doubleChannelCalculation_ != nullptr));
}