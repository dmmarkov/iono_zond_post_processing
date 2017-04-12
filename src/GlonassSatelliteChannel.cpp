/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GlonassSatelliteChannel.cpp
 * Author: markov
 */

#include "GlonassSatelliteChannel.hpp"
#include "gnss_const.hpp"

#ifndef FAKE_CALCULATION
#include "S4Calculation.hpp"
#include "SigmaPhiCalculation.hpp"
#else
#include "FakeS4Calculation.hpp"
#include "FakeSigmaPhiCalculation.hpp"
#endif

GlonassSatelliteChannel::GlonassSatelliteChannel(ISatellite* parentSatellite, SatelliteChannelType channelType, char glonassFrequency) : SatelliteChannel() {
    parentSatellite_ = parentSatellite;
    channelType_ = channelType;
    glonassFrequency_ = glonassFrequency;

    switch (channelType) {
        case SatelliteChannelType::primary:
        {
            //calculate working frequency for this channel
            frequency_ = GLO_L1_BASE_FREQUECY + GLO_L1_DELTA_FREQUECY * glonassFrequency_;
            waveLength_ = SPEED_OF_LIGHT / frequency_;
            //just set channel name
            channelName_ = "L1";
            break;
        }
        case SatelliteChannelType::secondary:
        {
            //calculate working frequency for this channel
            frequency_ = GLO_L2_BASE_FREQUECY + GLO_L2_DELTA_FREQUECY * glonassFrequency_;
            waveLength_ = SPEED_OF_LIGHT / frequency_;
            //just set channel name
            channelName_ = "L2";
            break;
        }
        default:
        {
            assert(false);
        }
    }

    fullChannelName_ = parentSatellite_->getSatelliteName() + " " + channelName_;

#ifndef FAKE_CALCULATION
    s4Calculation_.store(new S4Calculation(this));
    sigmaPhiCalculation_.store(new SigmaPhiCalculation(this));
#else
    s4Calculation_.store(new FakeS4Calculation(this));
    sigmaPhiCalculation_.store(new FakeSigmaPhiCalculation(this));
#endif

    createPlotData();
    Logger::getInstance().info("Created channel " + getFullChannelName());
}

GlonassSatelliteChannel::~GlonassSatelliteChannel() {
    IS4Calculation* s4 = s4Calculation_;
    delete s4;
    
    ISigmaPhiCalculation* sigma = sigmaPhiCalculation_;
    delete sigma;

    Logger::getInstance().info("Destroyed channel " + getFullChannelName());
}

char
GlonassSatelliteChannel::getGlonassFrequency() {
    return glonassFrequency_;
}