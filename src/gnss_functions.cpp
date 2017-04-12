#include "gnss_functions.hpp"
#include "Logger.hpp"
#include "gnss_const.hpp"
#include "server_consts.hpp"

PTime
getLogTime(unsigned short week, unsigned int milliseconds) {
    int64_t leaps[17] = {46828800, 78364801, 109900802,
        173059203, 252028804, 315187205,
        346723206, 393984007, 425520008,
        457056009, 504489610, 551750411,
        599184012, 820108813, 914803214,
        1025136015, 1119744016};

    double cur_seconds = (double) week * 604800.0 + (double) milliseconds / 1000.0;

    size_t leap_count = 0;
    for (size_t i = 0; i < 17; i++) {
        if (leaps[i] >= cur_seconds) {
            leap_count++;
        }
    }

    cur_seconds += leap_count;

    for (size_t i = 0; i < 17; i++) {
        if (leaps[i] >= (int64_t) trunc(cur_seconds)) {
            cur_seconds += 0.5;
        }
    }

    boost::posix_time::ptime time_epoch(boost::gregorian::date(1980, 1, 6));

    PTime newDateTime(time_epoch + boost::posix_time::milliseconds(cur_seconds * 1000));

    return newDateTime;
}

long int 
getMillisecondsTime(unsigned short week, unsigned int milliseconds) {
    return (long int)week * 604800000 + (long int)milliseconds;
}

SatelliteSystem
getSatelliteSystemType(unsigned char satelliteSystem) {
    switch (satelliteSystem) {
        case 0:
            return SatelliteSystem::GPS;
        case 1:
            return SatelliteSystem::GLONASS;
        default:
            return SatelliteSystem::Other; //it's for others satellite systems, that not use for today
    }

}

int
getSatelliteId(SatelliteSystem satelliteSystem, unsigned char satelliteId, bool isRangeSource) {
    //normal number of satellite inside of satellite group
    //today only range differ from other satellites
    //special const for drawing adds when creates satellite string name

    int satelliteNumber = satelliteId;

    if (isRangeSource && (satelliteSystem == SatelliteSystem::GLONASS)) {
        satelliteNumber -= GLONASS_SATELLITE_RANGE_CONST;
    }

    return satelliteNumber;
}

SatelliteChannelType
getChannelType(SatelliteSystem satelliteSystem, unsigned char signalType, bool isRangeSource) {

    SatelliteChannelType result = SatelliteChannelType::third;

    if (isRangeSource) {
        if (satelliteSystem == SatelliteSystem::GPS) {
            switch (signalType) {
                case 0:
                    result = SatelliteChannelType::primary;
                    break;
                case 5:
                    //it's bad idea
                    //TODO fix it if you will be use the third channel
                    result = SatelliteChannelType::third;
                    break;
                case 9:
                    result = SatelliteChannelType::secondary;
                    break;
                case 14:
                    result = SatelliteChannelType::third;
                    break;
                default:
                    assert(false);
            }
        }
        if (satelliteSystem == SatelliteSystem::GLONASS) {
            switch (signalType) {
                case 0:
                    result = SatelliteChannelType::primary;
                    break;
                case 5:
                    result = SatelliteChannelType::secondary;
                    break;
                default:
                    assert(false);
            }
        }
        if (satelliteSystem == SatelliteSystem::Other) {
            assert(false);
        }
    } else {
        if (satelliteSystem == SatelliteSystem::GPS) {
            switch (signalType) {
                case 1:
                    result = SatelliteChannelType::primary;
                    break;
                case 4:
                    //it's bad idea
                    //TODO fix it if you will be use the third channel
                    result = SatelliteChannelType::third;
                    break;
                case 6:
                    result = SatelliteChannelType::secondary;
                    break;
                case 7:
                    result = SatelliteChannelType::third;
                    break;
                default:
                    assert(false);
            }
        }
        if (satelliteSystem == SatelliteSystem::GLONASS) {
            switch (signalType) {
                case 1:
                    result = SatelliteChannelType::primary;
                    break;
                case 4:
                    result = SatelliteChannelType::secondary;
                    break;
                default:
                    assert(false);
            }
        }
        if (satelliteSystem == SatelliteSystem::Other) {
            assert(false);
        }
    }

    return result;
}

void
ecef2lla(EcefCoordiantes& ecef, LlaCoordiantes& lla) {
    double B = 0;
    double d = 0;
    double e = 0;
    double f = 0;
    double g = 0;
    double p = 0;
    double q = 0;
    double r = 0;
    double t = 0;
    double v = 0;
    double x = ecef.x;
    double y = ecef.y;
    double z = ecef.z;
    double zlong = 0;
    double A = RADIUS_EQUATOR_OF_THE_EARTH;
    double FL = EARTH_FLATTENING_FACTOR;

    const double ZERO = 0.0;
    const double ONE = 1.0;
    const double TWO = 2.0;
    const double THREE = 3.0;
    const double FOUR = 4.0;
    const double twopi = TWO * PI;
    const double rad_to_deg = 360.0 / twopi;

    /*
     *   1.0 compute semi-minor axis and set sign to that of z in order
     *       to get sign of Phi correct
     */
    B = A * (1.0 - FL);
    if (z < 0.0)
        B = -B;
    /*
     *   2.0 compute intermediate values for latitude
     */
    r = sqrt(x * x + y * y);
    e = (B * z - (A * A - B * B)) / (A * r);
    f = (B * z + (A * A - B * B)) / (A * r);
    /*
     *   3.0 find solution to:
     *       t^4 + 2*E*t^3 + 2*F*t - 1 = 0
     */
    p = (FOUR / THREE) * (e * f + ONE);
    q = TWO * (e * e - f * f);
    d = p * p * p + q*q;

    if (d >= ZERO) {
        v = pow((sqrt(d) - q), (ONE / THREE))
                - pow((sqrt(d) + q), (ONE / THREE));
    } else {
        v = TWO * sqrt(-p)
                * cos(acos(q / (p * sqrt(-p))) / THREE);
    }
    /*
     *   4.0 improve v
     *       NOTE: not really necessary unless point is near pole
     */
    if (v * v < fabs(p)) {
        v = -(v * v * v + TWO * q) / (THREE * p);
    }
    g = (sqrt(e * e + v) + e) / TWO;
    t = sqrt(g * g + (f - v * g) / (TWO * g - e)) - g;

    lla.latitude = atan((A * (ONE - t * t)) / (TWO * B * t));
    /*
     *   5.0 compute height above ellipsoid
     */
    lla.altitude = (r - A * t) * cos(lla.latitude) + (z - B) * sin(lla.latitude);
    /*
     *   6.0 compute longitude east of Greenwich
     */
    zlong = atan2(y, x);
//    if (zlong < ZERO)
//        zlong = zlong + twopi;

    lla.longitude = zlong;
    /*
     *   7.0 convert latitude and longitude to degrees
     */
    lla.latitude = lla.latitude * rad_to_deg;
    lla.longitude = lla.longitude * rad_to_deg;
    
    if (ecef.z < 0 && lla.latitude > 0) {
        lla.latitude = lla.latitude * -1;
    }
    
//    if (ecef.y < ZERO) {
//        lla.longitude = lla.longitude - 360.0;
//    }
}

void
lla2ecef(LlaCoordiantes& lla, EcefCoordiantes& ecef) {
    const double ONE = 1.0;
    const double TWO = 2.0;

    double A = RADIUS_EQUATOR_OF_THE_EARTH;
    double FL = EARTH_FLATTENING_FACTOR;

    const double twopi = TWO * PI;
    double deg_to_rad = twopi / 360.0;

    double flatfn = (TWO - FL) * FL;
    double funsq = (ONE - FL)*(ONE - FL);
    double g1;
    double g2;
    double lat_rad = deg_to_rad * lla.latitude;
    double lon_rad = deg_to_rad * lla.longitude;
    double sin_lat;

    sin_lat = sin(lat_rad);

    g1 = A / sqrt(ONE - flatfn * sin_lat * sin_lat);
    g2 = g1 * funsq + lla.altitude;
    g1 = g1 + lla.altitude;

    ecef.x = g1 * cos(lat_rad);
    ecef.y = ecef.x * sin(lon_rad);
    ecef.x = ecef.x * cos(lon_rad);
    ecef.z = g2 * sin_lat;
}