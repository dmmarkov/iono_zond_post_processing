/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <math.h>

#include "gnss_const.hpp"

const double SPEED_OF_LIGHT = 299792458.0;
const double GPS_L1_FREQUENCY = 1575420000.0;
const double GPS_L1_WAVE = SPEED_OF_LIGHT / GPS_L1_FREQUENCY;
const double GPS_L2_FREQUENCY = 1227600000.0;
const double GPS_L2_WAVE = SPEED_OF_LIGHT / GPS_L2_FREQUENCY;
const double GLO_L1_BASE_FREQUECY = 1602000000.0;
const double GLO_L1_DELTA_FREQUECY = 562500.0;
const double GLO_L2_BASE_FREQUECY = 1246000000.0;
const double GLO_L2_DELTA_FREQUECY = 437500.0;

const long double PI = acos(-1);
const long double ELEMENTARY_ELECTRIC_CHARGE = 1.60217656535 * pow(10, -19);
const long double MASS_ELECTRON = 9.1093829140 * pow(10, -31);
const long double ELECTRON_CHARGE_TO_MASS = ELEMENTARY_ELECTRIC_CHARGE / MASS_ELECTRON;
const long double E0 = 1.0 / 36.0 / PI * pow(10, -9);
const long double TEC_G = ELEMENTARY_ELECTRIC_CHARGE * ELECTRON_CHARGE_TO_MASS / 8.0 / PI / PI / E0;
const long double K_BOLTZMAN = 1.38 * pow(10, -23);
const long double TECU_KOEFFICIENT = 10000000000000000.0;

const unsigned int CALCULATE_SATELLITE_THREAD_SLEEP_MS = 10000;
const unsigned int CALCULATE_CHANNEL_THREAD_SLEEP_MS = 100;
const unsigned int CLEAR_DATA_THREAD_SLEEP_M = 10;
const unsigned int SOCKET_ASK_SLEEP_MS = 100;
const unsigned int SOCKET_ACP_SLEEP_MS = 1000;

const unsigned int PARSE_THREAD_SLEEP_RAW_NETWORK_DATA_MS = 1;
const unsigned int PARSE_THREAD_SLEEP_RAW_RANGE_MS = 10;
const unsigned int PARSE_THREAD_SLEEP_RAW_PSRPOS_MS = 1000;
const unsigned int PARSE_THREAD_SLEEP_RAW_PSRXYZ_MS = 300;
const unsigned int PARSE_THREAD_SLEEP_RAW_SATVIS2_MS = 1000;
const unsigned int PARSE_THREAD_SLEEP_RAW_ISMRAWOBS_MS = 60000;
const unsigned int PARSE_THREAD_SLEEP_RAW_ISMRAWTEC_MS = 60000;
const unsigned int PARSE_THREAD_SLEEP_RAW_SATXYZ2_MS = 10;
const unsigned int PARSE_THREAD_SLEEP_RAW_ISMDETOBS_MS = 200;
const unsigned int PARSE_THREAD_SLEEP_RAW_ISMREDOBS_MS = 60000;
const unsigned int PARSE_THREAD_SLEEP_START_MS = 60000;

const float DOUBLE_CHANNEL_MIN_LOCKTIME = 120.0;
const float S4_MIN_LOCKTIME = 120.0;
const float SIGMA_PHI_MIN_LOCKTIME = 300.0;

const unsigned int MIN_SATVIS_COUNT = 1;
const unsigned char GLONASS_SATELLITE_RANGE_CONST = 37;
const char GLONASS_FREQUENCY_RANGE_CONST = 7;
const double MIN_ELEVATION_ANGLE = 14.99;
const double S4_IRREGULATION_VALUE = 0.3;

const size_t S4_COUNTER = 3000;
const size_t SIGMA_PHI_COUNTER = 3000;

// WGS84 ellipsoid constants
const long double RADIUS_EQUATOR_OF_THE_EARTH = 6378137.0; 
const long double EARTH_FLATTENING_FACTOR = 0.00335281068118;
//const long double ECCENTRICITY_OF_THE_EARTH = 8.1819190842622e-2;  // eccentricity
//
//const long double WGS84_ASQ = pow(RADIUS_OF_THE_EARTH, 2.0);
//const long double WGS84_ESQ = pow(ECCENTRICITY_OF_THE_EARTH, 2.0);