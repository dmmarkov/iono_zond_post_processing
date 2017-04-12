/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GlonassSatellite.hpp
 * Author: markov
 */

#ifndef GLONASSSATELLITE_HPP
#define GLONASSSATELLITE_HPP

#include "Satellite.hpp"
#include "GlonassSatelliteChannel.hpp"

class GlonassSatellite : public Satellite {
public:
    explicit GlonassSatellite(int satelliteID, char frequency);
    virtual ~GlonassSatellite();
    
    static char getGlonassFrequency(char frequency, bool isRangeSource);
    char getGlonassFrequency();
    
    virtual SatelliteSystem getSatelliteSystem() override;
    virtual void stopThreads() override;
protected:
    virtual void createSatelliteChannels(double elevationAngle, unsigned short week, unsigned int milliseconds) override;
    virtual void destroySatelliteChannels(double elevationAngle, unsigned short week, unsigned int milliseconds) override;
    virtual bool isChannelsExists() override;
private:
    char frequency_;
};

#endif /* GLONASSSATELLITE_HPP */

