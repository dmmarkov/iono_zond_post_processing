/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ISatelliteChannel.hpp
 * Author: markov
 */

#ifndef ISATELLITECHANNEL_HPP
#define ISATELLITECHANNEL_HPP

#include "GnssLogTime.hpp"
#include "GnssCommonTypes.hpp"
#include "ISatellite.hpp"


class ISatelliteChannel {
public:
    virtual ~ISatelliteChannel() {
        
    }
    
    virtual std::string getSatelliteName() = 0;
    virtual std::string getChannelName() = 0;
    virtual std::string getFullChannelName() = 0;
    virtual char getGlonassFrequency() = 0;
    virtual ISatellite* getSatellite() = 0;

    virtual void pushDetrendedPower(double detrendedPower, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushCarrierToNoiseAndLockTime(double carrierToNoise, double lockTime, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushDetrendedPhase(double detrendedPhase, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushNovatelS4(double novatelS4, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushNovatelSigmaPhi60(double novatelSigmaPhi60, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushNovatelCodeCarrierDivergence(double novatelCodeCarrierDivergence, unsigned short week, unsigned int milliseconds) = 0;
    
    virtual double getFrequency() = 0;
    virtual double getWaveLength() = 0;
    
    virtual void stopThreads() = 0;
    
    virtual void closePlotData() = 0;
    
    virtual void forceCloseOutDataFiles(GnssLogTime& time) = 0;
};

#endif /* ISATELLITECHANNEL_HPP */

