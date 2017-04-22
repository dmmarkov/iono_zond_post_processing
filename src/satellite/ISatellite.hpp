/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ISatellite.hpp
 * Author: markov
 */

#ifndef ISATELLITE_HPP
#define ISATELLITE_HPP

#include "GnssCommonTypes.hpp"
#include "GnssLogTime.hpp"

class ISatellite {
public:
    virtual ~ISatellite() {
        
    }
    
    virtual int getSatelliteId() = 0;
    virtual SatelliteSystem getSatelliteSystem() = 0;

    virtual std::string getSatelliteName() = 0;
    
    virtual void pushDetrendedPower(SatelliteChannelType channelType, double detrendedPower, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushCarrierToNoiseAndLockTime(SatelliteChannelType channelType, double carrierToNoise, double lockTime, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushDetrendedPhase(SatelliteChannelType channelType, double detrendedPhase, unsigned short week, unsigned int milliseconds) = 0;
    
    virtual void pushNovatelS4(SatelliteChannelType channelType, double novatelS4, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushNovatelSigmaPhi60(SatelliteChannelType channelType, double novatelSigmaPhi60, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushNovatelCodeCarrierDivergence(SatelliteChannelType channelType, double novatelCodeCarrierDivergence, unsigned short week, unsigned int milliseconds) = 0;
    
    virtual void pushSatVisParams(double elevationAngle, double azimuthAngle, unsigned short week, unsigned int milliseconds) = 0;   
    
    virtual void pushSatelliteXyz(double x, double y, double z, unsigned short week, unsigned int milliseconds) = 0;
    
    virtual void pushPsrPhaseLockTime(SatelliteChannelType channelType, double pseudorange, double carrierPhase, double lockTime, double CarrierToNoise, unsigned short week, unsigned int milliseconds) = 0;
    virtual void stopThreads() = 0;
    virtual void forceCloseOutDataFiles(GnssLogTime& time) = 0;
};

#endif /* ISATELLITE_HPP */

