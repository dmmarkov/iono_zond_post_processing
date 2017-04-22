/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PashitsevSatelliteChannel.hpp
 * Author: dmitri
 *
 * Created on 12 Июнь 2016 г., 15:37
 */

#ifndef PASHITSEVSATELLITECHANNEL_HPP
#define PASHITSEVSATELLITECHANNEL_HPP

#include "ISatelliteChannel.hpp"
#include "ISatellite.hpp"

namespace pashintsev {
    class PashitsevSatelliteChannel;
}

class pashintsev::PashitsevSatelliteChannel : public ISatelliteChannel{
public:
    explicit PashitsevSatelliteChannel(ISatellite* parentSatellite);    
    virtual ~PashitsevSatelliteChannel();
    
    virtual std::string getSatelliteName() override;
    virtual std::string getChannelName() override;
    virtual std::string getFullChannelName() override;
    virtual char getGlonassFrequency() override;
    virtual ISatellite* getSatellite() override;

    virtual void pushDetrendedPower(double detrendedPower, unsigned short week, unsigned int milliseconds) override;
    virtual void pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) override;
    virtual void pushCarrierToNoiseAndLockTime(double carrierToNoise, double lockTime, unsigned short week, unsigned int milliseconds) override;
    virtual void pushDetrendedPhase(double detrendedPhase, unsigned short week, unsigned int milliseconds) override;
    virtual void pushNovatelS4(double novatelS4, unsigned short week, unsigned int milliseconds) override;
    virtual void pushNovatelSigmaPhi60(double novatelSigmaPhi60, unsigned short week, unsigned int milliseconds) override;
    virtual void pushNovatelCodeCarrierDivergence(double novatelCodeCarrierDivergence, unsigned short week, unsigned int milliseconds) override;
    
    virtual double getFrequency() override;
    virtual double getWaveLength() override;
    
    virtual void stopThreads() override;
    
    virtual void closePlotData() override;
    
    virtual void forceCloseOutDataFiles(GnssLogTime& time) override;
private:
    ISatellite* parentSatellite_;
    std::string channelName_;
    
    PashitsevSatelliteChannel(const PashitsevSatelliteChannel& orig) = delete;
    
};

#endif /* PASHITSEVSATELLITECHANNEL_HPP */

