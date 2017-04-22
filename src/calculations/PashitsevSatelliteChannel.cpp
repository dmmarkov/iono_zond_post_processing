/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PashitsevSatelliteChannel.cpp
 * Author: dmitri
 * 
 * Created on 12 Июнь 2016 г., 15:37
 */

#include "gnss_const.hpp"
#include "pashintsev_const.hpp"

#include "PashitsevSatelliteChannel.hpp"

pashintsev::PashitsevSatelliteChannel::PashitsevSatelliteChannel(ISatellite* parentSatellite) {
    parentSatellite_ = parentSatellite;
    channelName_ = "Pashintsev";
}

pashintsev::PashitsevSatelliteChannel::~PashitsevSatelliteChannel() {
}

std::string
pashintsev::PashitsevSatelliteChannel::getSatelliteName() {
    return parentSatellite_->getSatelliteName();
}

std::string
pashintsev::PashitsevSatelliteChannel::getChannelName() {
    return channelName_;
}

std::string
pashintsev::PashitsevSatelliteChannel::getFullChannelName() {
    return parentSatellite_->getSatelliteName() + " " + channelName_;
}

char
pashintsev::PashitsevSatelliteChannel::getGlonassFrequency() {
    return 0;
}

ISatellite* 
pashintsev::PashitsevSatelliteChannel::getSatellite() {
    return parentSatellite_;
}

void
pashintsev::PashitsevSatelliteChannel::pushDetrendedPower(double detrendedPower, unsigned short week, unsigned int milliseconds) {
    (void) detrendedPower;
    (void) week;
    (void) milliseconds;
}

void
pashintsev::PashitsevSatelliteChannel::pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) {
    (void) elevationAngle;
    (void) week;
    (void) milliseconds;
}

void
pashintsev::PashitsevSatelliteChannel::pushCarrierToNoiseAndLockTime(double carrierToNoise, double lockTime, unsigned short week, unsigned int milliseconds) {
    (void) carrierToNoise;
    (void) week;
    (void) milliseconds;
}

void
pashintsev::PashitsevSatelliteChannel::pushDetrendedPhase(double detrendedPhase, unsigned short week, unsigned int milliseconds) {
    (void) detrendedPhase;
    (void) week;
    (void) milliseconds;
}

void
pashintsev::PashitsevSatelliteChannel::pushNovatelS4(double novatelS4, unsigned short week, unsigned int milliseconds) {
    (void) novatelS4;
    (void) week;
    (void) milliseconds;
}

void
pashintsev::PashitsevSatelliteChannel::pushNovatelSigmaPhi60(double novatelSigmaPhi60, unsigned short week, unsigned int milliseconds) {
    (void) novatelSigmaPhi60;
    (void) week;
    (void) milliseconds;
}

void
pashintsev::PashitsevSatelliteChannel::pushNovatelCodeCarrierDivergence(double novatelCodeCarrierDivergence, unsigned short week, unsigned int milliseconds) {
    (void) novatelCodeCarrierDivergence;
    (void) week;
    (void) milliseconds;
}

double
pashintsev::PashitsevSatelliteChannel::getFrequency() {
    return CALCULATION_FREQUENCY;
}

double
pashintsev::PashitsevSatelliteChannel::getWaveLength() {
    return  SPEED_OF_LIGHT / getFrequency();
}

void
pashintsev::PashitsevSatelliteChannel::stopThreads() {

}

void
pashintsev::PashitsevSatelliteChannel::closePlotData() {

}

void
pashintsev::PashitsevSatelliteChannel::forceCloseOutDataFiles(GnssLogTime& time) {

}
