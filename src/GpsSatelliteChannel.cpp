/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GpsSatelliteChannel.cpp
 * Author: markov
 */

#include "GpsSatelliteChannel.hpp"
#include "gnss_const.hpp"
#ifndef FAKE_CALCULATION
#include "S4Calculation.hpp"
#include "SigmaPhiCalculation.hpp"
#else
#include "FakeS4Calculation.hpp"
#include "FakeSigmaPhiCalculation.hpp"
#endif

GpsSatelliteChannel::GpsSatelliteChannel(ISatellite* parentSatellite, SatelliteChannelType channelType) : SatelliteChannel() {
    parentSatellite_ = parentSatellite;

    switch (channelType) {
        case SatelliteChannelType::primary:
        {
            frequency_ = GPS_L1_FREQUENCY;
            waveLength_ = GPS_L1_WAVE;
            channelName_ = "L1";
            break;
        }
        case SatelliteChannelType::secondary:
        {
            frequency_ = GPS_L2_FREQUENCY;
            waveLength_ = GPS_L2_WAVE;
            channelName_ = "L2";
            break;
        }
        case SatelliteChannelType::third:
        {
            //this is not used now, so only asser here
            assert(false);
            break;
        }
        default:
        {
            assert(false);
        }
    }

    fullChannelName_ = parentSatellite_->getSatelliteName() + " " + channelName_;

    //For GPS only L1 can be used for calculation of data
    if (channelType == SatelliteChannelType::primary) {
#ifndef FAKE_CALCULATION
        s4Calculation_.store(new S4Calculation(this));
        sigmaPhiCalculation_.store(new SigmaPhiCalculation(this));
#else
        s4Calculation_.store(new FakeS4Calculation(this));
        sigmaPhiCalculation_.store(new FakeSigmaPhiCalculation(this));
#endif
    } else {
        s4Calculation_ = nullptr;
        sigmaPhiCalculation_ = nullptr;
    }
    
    createPlotData();
    
    Logger::getInstance().info("Created channel " + getFullChannelName());
}

GpsSatelliteChannel::~GpsSatelliteChannel() {
    if (s4Calculation_ != nullptr) {
        IS4Calculation* s4 = s4Calculation_;
        delete s4;
    }
    
    if (sigmaPhiCalculation_ != nullptr) {
        ISigmaPhiCalculation* sigma = sigmaPhiCalculation_;
        delete sigma;
    }

    Logger::getInstance().info("Destroyed channel " + getFullChannelName());

}

char
GpsSatelliteChannel::getGlonassFrequency() {
    return 0;
}

