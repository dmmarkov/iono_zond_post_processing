/*
 * File:   Satellite.cpp
 * Author: markov
 */

#include <cassert>
#include <fstream>

#include <boost/foreach_fwd.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>

#include "Satellite.hpp"
#include "gnss_const.hpp"
#include "Logger.hpp"
#include "GlobalConfiguration.hpp"
#include "common_functions.hpp"
#include "server_consts.hpp"
#include "SvmPredictionData.hpp"

Satellite::Satellite() : satelliteId_(0),
satelliteName_(""),
firstChannel_(nullptr),
secondChannel_(nullptr),
doubleChannelCalculation_(nullptr),
isEnabledProcessing_(false),
lockPrintPlots_(LOCK_PRINT_PLOTS) {

}

Satellite::~Satellite() {

}

int
Satellite::getSatelliteId() {
    return satelliteId_;
}

std::string
Satellite::getSatelliteName() {
    return satelliteName_;
}

void
Satellite::pushSatelliteXyz(double x, double y, double z, unsigned short week, unsigned int milliseconds) {
    if (GlobalConfiguration::getInstance().getIsReceiverXyzSet()) {
        //calculating of elevation angle by using static coordinates of station and the satellite coordinates that are parameters of procedure
        long double rX = GlobalConfiguration::getInstance().getReceiverX();
        long double rY = GlobalConfiguration::getInstance().getReceiverY();
        long double rZ = GlobalConfiguration::getInstance().getReceiverZ();
        long double numerator = std::fabs(rX * (rX - x) + rY * (rY - y) + rZ * (rZ - z));
        long double l1 = sqrt(pow(rX, 2) + pow(rY, 2) + pow(rZ, 2));
        long double l2 = sqrt(pow(rX - x, 2) + pow(rY - y, 2) + pow(rZ - z, 2));
        long double sinElevationAnle = numerator / l1 / l2;

        double elevationAngle = asin(sinElevationAnle) * 180.0 / PI;

        pushCalculatedElevationAngle(elevationAngle, week, milliseconds);
    }
}

void
Satellite::pushPsrPhaseLockTime(SatelliteChannelType channelType, double pseudorange, double carrierPhase, double lockTime, double CarrierToNoise, unsigned short week, unsigned int milliseconds) {
    if (isEnabledProcessing_ && (doubleChannelCalculation_ != nullptr)) {
        IDoubleChannelCalculation* dchannel = doubleChannelCalculation_;
        switch (channelType) {
            case SatelliteChannelType::primary:
                dchannel->pushPsrPhaseLockTime1(pseudorange, carrierPhase, lockTime, CarrierToNoise, week, milliseconds);
                break;
            case SatelliteChannelType::secondary:
                dchannel->pushPsrPhaseLockTime2(pseudorange, carrierPhase, lockTime, CarrierToNoise, week, milliseconds);
                break;
            case SatelliteChannelType::third:
//                Logger::getInstance().error("Third channel");
                //TODO fix it
                //not usefull for today, so ignore it
                break;
            default:
                assert(false);
        }
    }
}

void
Satellite::pushDetrendedPower(SatelliteChannelType channelType, double detrendedPower, unsigned short week, unsigned int milliseconds) {
    if (isEnabledProcessing_ && isChannelsExists()) {
        switch (channelType) {
            case SatelliteChannelType::primary:
            {
                ISatelliteChannel* channel = firstChannel_;
                channel->pushDetrendedPower(detrendedPower, week, milliseconds);
            }
                break;
            case SatelliteChannelType::secondary:
            {
                ISatelliteChannel* channel = secondChannel_;
                channel->pushDetrendedPower(detrendedPower, week, milliseconds);
            }
                break;
            case SatelliteChannelType::third:
//                Logger::getInstance().error("Third channel");
                //TODO fix it
                //not usefull for today, so ignore it
                break;
            default:
                assert(false);
        }
    }
}

void
Satellite::pushCarrierToNoiseAndLockTime(SatelliteChannelType channelType, double carrierToNoise, double lockTime, unsigned short week, unsigned int milliseconds) {
    if (isEnabledProcessing_ && isChannelsExists()) {
        switch (channelType) {
            case SatelliteChannelType::primary:
            {
                ISatelliteChannel* channel = firstChannel_;
                channel->pushCarrierToNoiseAndLockTime(carrierToNoise, lockTime, week, milliseconds);
            }
                break;
            case SatelliteChannelType::secondary:
            {
                ISatelliteChannel* channel = secondChannel_;
                channel->pushCarrierToNoiseAndLockTime(carrierToNoise, lockTime, week, milliseconds);
            }
                break;
            case SatelliteChannelType::third:
//                Logger::getInstance().error("Third channel");
                //TODO fix it
                //not usefull for today, so ignore it
                break;
            default:
                assert(false);
        }
    }
}

void
Satellite::pushDetrendedPhase(SatelliteChannelType channelType, double detrendedPhase, unsigned short week, unsigned int milliseconds) {
    if (isEnabledProcessing_ && isChannelsExists()) {
        switch (channelType) {
            case SatelliteChannelType::primary:
            {
                ISatelliteChannel* channel = firstChannel_;
                channel->pushDetrendedPhase(detrendedPhase, week, milliseconds);
            }
                break;
            case SatelliteChannelType::secondary:
            {
                ISatelliteChannel* channel = secondChannel_;
                channel->pushDetrendedPhase(detrendedPhase, week, milliseconds);
            }
                break;
            case SatelliteChannelType::third:
//                Logger::getInstance().error("Third channel");
                //TODO fix it
                break;
            default:
                assert(false);
        }
    }
}

void
Satellite::pushNovatelS4(SatelliteChannelType channelType, double novatelS4, unsigned short week, unsigned int milliseconds) {
    if (isEnabledProcessing_ && isChannelsExists()) {
        switch (channelType) {
            case SatelliteChannelType::primary:
            {
                ISatelliteChannel* channel = firstChannel_;
                channel->pushNovatelS4(novatelS4, week, milliseconds);
            }
                break;
            case SatelliteChannelType::secondary:
            {
                ISatelliteChannel* channel = secondChannel_;
                channel->pushNovatelS4(novatelS4, week, milliseconds);
            }
                break;
            case SatelliteChannelType::third:
//                Logger::getInstance().error("Third channel");
                //TODO fix it
                break;
            default:
                assert(false);
        }
    }
}

void
Satellite::pushNovatelSigmaPhi60(SatelliteChannelType channelType, double novatelSigmaPhi60, unsigned short week, unsigned int milliseconds) {
    if (isEnabledProcessing_ && isChannelsExists()) {
        switch (channelType) {
            case SatelliteChannelType::primary:
            {
                ISatelliteChannel* channel = firstChannel_;
                channel->pushNovatelSigmaPhi60(novatelSigmaPhi60, week, milliseconds);
            }
                break;
            case SatelliteChannelType::secondary:
            {
                ISatelliteChannel* channel = secondChannel_;
                channel->pushNovatelSigmaPhi60(novatelSigmaPhi60, week, milliseconds);
            }
                break;
            case SatelliteChannelType::third:
//                Logger::getInstance().error("Third channel");
                //TODO fix it
                break;
            default:
                assert(false);
        }
    }
}

void
Satellite::pushNovatelCodeCarrierDivergence(SatelliteChannelType channelType, double novatelCodeCarrierDivergence, unsigned short week, unsigned int milliseconds) {
    if (isEnabledProcessing_ && isChannelsExists()) {
        switch (channelType) {
            case SatelliteChannelType::primary:
            {
                ISatelliteChannel* channel = firstChannel_;
                channel->pushNovatelCodeCarrierDivergence(novatelCodeCarrierDivergence, week, milliseconds);
            }
                break;
            case SatelliteChannelType::secondary:
            {
                ISatelliteChannel* channel = secondChannel_;
                channel->pushNovatelCodeCarrierDivergence(novatelCodeCarrierDivergence, week, milliseconds);
            }
                break;
            case SatelliteChannelType::third:
//                Logger::getInstance().error("Third channel");
                //TODO fix it
                break;
            default:
                assert(false);
        }
    }
}

void
Satellite::createSatelliteChannels(double elevationAngle, unsigned short week, unsigned int milliseconds) {
    firstWeek_ = week;
    firstMilliseconds_ = milliseconds;

    SvmPredictionData::getInstance().addSatellite(this, week, milliseconds);
}

void
Satellite::destroySatelliteChannels(double elevationAngle, unsigned short week, unsigned int milliseconds) {
    std::fstream outData;
    outData.open(satelliteName_, std::ios_base::out | std::ios_base::app);
    outData << firstWeek_ << ";" << firstMilliseconds_ << ";" << week << ";" << milliseconds << std::endl;
    outData.close();
}

void
Satellite::pushSatVisParams(double elevationAngle, double azimuthAngle, unsigned short week, unsigned int milliseconds) {
    if ((elevationAngle > 0) && (!isChannelsExists())) {
        createSatelliteChannels(elevationAngle, week, milliseconds);

    }
    if ((elevationAngle < 0) && isChannelsExists()) {
        destroySatelliteChannels(elevationAngle, week, milliseconds);
    }
    if (isChannelsExists()) {
        if (elevationAngle > MIN_ELEVATION_ANGLE) {
            isEnabledProcessing_ = true;
        } else {
            if (isEnabledProcessing_) {
                ISatelliteChannel* channel = firstChannel_;
                channel->closePlotData();

                channel = secondChannel_;
                channel->closePlotData();

                IDoubleChannelCalculation* dchannel = doubleChannelCalculation_;
                dchannel->closePlotData();

                isEnabledProcessing_ = false;
            }
        }
    }
}

void
Satellite::pushCalculatedElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) {
    if (isChannelsExists()) {
        if (isEnabledProcessing_) {
            ISatelliteChannel* channel = firstChannel_;
            channel->pushElevationAngle(elevationAngle, week, milliseconds);

            channel = secondChannel_;
            channel->pushElevationAngle(elevationAngle, week, milliseconds);

            if (doubleChannelCalculation_ != nullptr) {
                IDoubleChannelCalculation* dchannel = doubleChannelCalculation_;
                dchannel->pushElevationAngle(elevationAngle, week, milliseconds);
            }

            SvmPredictionData::getInstance().pushElevationAngle(this,
                                                                elevationAngle,
                                                                week,
                                                                milliseconds);
        }
    }
}

void
Satellite::forceCloseOutDataFiles(GnssLogTime& time) {
    if (isChannelsExists()) {
        if (isEnabledProcessing_) {
            isEnabledProcessing_ = false;
            ISatelliteChannel* channel = firstChannel_;
            channel->forceCloseOutDataFiles(time);

            channel = secondChannel_;
            channel->forceCloseOutDataFiles(time);

            IDoubleChannelCalculation* dchannel = doubleChannelCalculation_;
            dchannel->forceCloseOutDataFiles(time);
        }
    }
}