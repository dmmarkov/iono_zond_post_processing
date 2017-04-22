/* 
 * File:   gnss_functions.hpp
 * Author: markov
 */

#ifndef GNSS_FUNCTIONS_HPP
#define	GNSS_FUNCTIONS_HPP

#include "project_defines.hpp"
#include "GnssCommonTypes.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

struct EcefCoordiantes {
    double x;
    double y;
    double z;
};

struct LlaCoordiantes {
    double latitude;
    double longitude;
    double altitude;
};

PTime getLogTime(unsigned short week, unsigned int milliseconds) ;
long int getMillisecondsTime(unsigned short week, unsigned int milliseconds) ;

SatelliteSystem getSatelliteSystemType(unsigned char satelliteSystem) ;
int getSatelliteId(SatelliteSystem satelliteSystem, unsigned char satelliteId, bool isRangeSource) ;
SatelliteChannelType getChannelType(SatelliteSystem satelliteSystem, unsigned char signalType, bool isRangeSource) ;

void ecef2lla(EcefCoordiantes& ecef, LlaCoordiantes& lla);
void lla2ecef(LlaCoordiantes& lla, EcefCoordiantes& ecef);

#endif	/* GNSS_FUNCTIONS_HPP */

