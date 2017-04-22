/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GpsSatelliteChannel.hpp
 * Author: markov
 */

#ifndef GPSSATELLITECHANNEL_HPP
#define GPSSATELLITECHANNEL_HPP

#include <string>

#include "SatelliteChannel.hpp"
#include "ISatellite.hpp"

class GpsSatelliteChannel : public SatelliteChannel {
public:
    explicit GpsSatelliteChannel(ISatellite* parentSatellite, SatelliteChannelType channelType);

    virtual ~GpsSatelliteChannel();
    
    virtual char getGlonassFrequency() override;
private:
};

#endif /* GPSSATELLITECHANNEL_HPP */

