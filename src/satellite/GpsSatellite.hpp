/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GpsSatellite.hpp
 * Author: markov
 */

#ifndef GPSSATELLITE_HPP
#define GPSSATELLITE_HPP

#include "Satellite.hpp"
#include "GpsSatelliteChannel.hpp"

class GpsSatellite : public Satellite {
public:
    explicit GpsSatellite(int satelliteId);
    virtual ~GpsSatellite();
    
    virtual SatelliteSystem getSatelliteSystem() override;
    virtual void stopThreads() override;
protected:
    virtual void createSatelliteChannels(double elevationAngle, unsigned short week, unsigned int milliseconds) override;
    virtual void destroySatelliteChannels(double elevationAngle, unsigned short week, unsigned int milliseconds) override;
    virtual bool isChannelsExists() override;
private:
};

#endif /* GPSSATELLITE_HPP */

