/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GnssCommonTypes.hpp
 * Author: markov
 */

#ifndef GNSSCOMMONTYPES_HPP
#define GNSSCOMMONTYPES_HPP

#include <boost/date_time/posix_time/posix_time.hpp>

enum class SatelliteSystem {
    GPS, GLONASS, Other
};

enum class SatelliteChannelType {
    primary, secondary, third
};

typedef boost::posix_time::ptime PTime;

#endif /* GNSSCOMMONTYPES_HPP */

