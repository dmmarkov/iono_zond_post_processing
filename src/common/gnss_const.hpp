/* 
 * File:   gnss_const.hpp
 * Author: markov
 */

#ifndef GNSS_CONST_HPP
#define	GNSS_CONST_HPP

#include <cstddef>

#include "project_defines.hpp"

extern const double SPEED_OF_LIGHT;
extern const double GPS_L1_FREQUENCY;
extern const double GPS_L1_WAVE;
extern const double GPS_L2_FREQUENCY;
extern const double GPS_L2_WAVE;
extern const double GLO_L1_BASE_FREQUECY;
extern const double GLO_L1_DELTA_FREQUECY;
extern const double GLO_L2_BASE_FREQUECY;
extern const double GLO_L2_DELTA_FREQUECY;

extern const long double PI;
extern const long double ELEMENTARY_ELECTRIC_CHARGE;
extern const long double MASS_ELECTRON;
extern const long double ELECTRON_CHARGE_TO_MASS;
extern const long double E0;
extern const long double TEC_G;
extern const long double K_BOLTZMAN;
extern const long double TECU_KOEFFICIENT;

extern const unsigned int CALCULATE_SATELLITE_THREAD_SLEEP_MS;
extern const unsigned int CALCULATE_CHANNEL_THREAD_SLEEP_MS;
extern const unsigned int CLEAR_DATA_THREAD_SLEEP_M;
extern const unsigned int SOCKET_ASK_SLEEP_MS;
extern const unsigned int SOCKET_ACP_SLEEP_MS;

extern const unsigned int PARSE_THREAD_SLEEP_RAW_NETWORK_DATA_MS;
extern const unsigned int PARSE_THREAD_SLEEP_RAW_RANGE_MS;
extern const unsigned int PARSE_THREAD_SLEEP_RAW_PSRPOS_MS;
extern const unsigned int PARSE_THREAD_SLEEP_RAW_PSRXYZ_MS;
extern const unsigned int PARSE_THREAD_SLEEP_RAW_SATVIS2_MS;
extern const unsigned int PARSE_THREAD_SLEEP_RAW_ISMRAWOBS_MS;
extern const unsigned int PARSE_THREAD_SLEEP_RAW_ISMRAWTEC_MS;
extern const unsigned int PARSE_THREAD_SLEEP_RAW_SATXYZ2_MS;
extern const unsigned int PARSE_THREAD_SLEEP_RAW_ISMDETOBS_MS;
extern const unsigned int PARSE_THREAD_SLEEP_RAW_ISMREDOBS_MS;
extern const unsigned int PARSE_THREAD_SLEEP_START_MS;

extern const float DOUBLE_CHANNEL_MIN_LOCKTIME;
extern const float S4_MIN_LOCKTIME;
extern const float SIGMA_PHI_MIN_LOCKTIME;

extern const unsigned int MIN_SATVIS_COUNT;
extern const unsigned char GLONASS_SATELLITE_RANGE_CONST;
extern const char GLONASS_FREQUENCY_RANGE_CONST;
extern const double MIN_ELEVATION_ANGLE;
extern const double S4_IRREGULATION_VALUE;

extern const std::size_t S4_COUNTER;
extern const std::size_t SIGMA_PHI_COUNTER;

extern const long double RADIUS_EQUATOR_OF_THE_EARTH; 
extern const long double EARTH_FLATTENING_FACTOR;

#endif	/* GNSS_CONST_HPP */

