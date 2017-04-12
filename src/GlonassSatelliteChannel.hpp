/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GlonassSatelliteChannel.hpp
 * Author: markov
 */

#ifndef GLONASSSATELLITECHANNEL_HPP
#define GLONASSSATELLITECHANNEL_HPP

#include "SatelliteChannel.hpp"


class GlonassSatelliteChannel : public SatelliteChannel {
public:
    explicit GlonassSatelliteChannel(ISatellite* parentSatellite, SatelliteChannelType channelType, char glonassFrequency);
    virtual ~GlonassSatelliteChannel();
    
    virtual char getGlonassFrequency() override;
private:
    char glonassFrequency_;
};

#endif /* GLONASSSATELLITECHANNEL_HPP */

