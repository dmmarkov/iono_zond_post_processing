/*
 * File:   SatelliteGroup.cpp
 * Author: dmitri
 */

#include <thread>
#include <chrono>

#include <mutex>

#include "SatelliteGroup.hpp"
#include "Logger.hpp"
#include "gnss_const.hpp"
#include "GpsSatellite.hpp"
#include "GlonassSatellite.hpp"
#include "GnssCommonTypes.hpp"
#include "GlobalConfiguration.hpp"

SatelliteGroup::SatelliteGroup() : isPsrPosSet_(false) {

}

SatelliteGroup::~SatelliteGroup() {
    for (auto& it : satellites_) {
        it.second->stopThreads();
    }

    std::this_thread::sleep_for (std::chrono::seconds(5));

    for (auto& it : satellites_) {
        delete it.second;
    }
}

void
SatelliteGroup::writeISMRAWOBS(LogRawHeader* CurrentHeader, LogRawBodyISMRAWOBS* CurrentLog) {
    //blank funcion, because we don't use it for today, but use in the past
}

void
SatelliteGroup::writeISMRAWTEC(LogRawHeader* CurrentHeader, LogRawBodyISMRAWTEC* CurrentLog) {
    //blank funcion, because we don't use it for today, but use in the past
}

void
SatelliteGroup::writeSATXYZ2(LogRawHeader* CurrentHeader, LogRawBodySATXYZ2* CurrentLog) {
    ISatellite* currentSatellite = addSatellite(CurrentLog->satelliteID, CurrentLog->satelliteSystem, CurrentLog->glonassFrequency, false);
    if (currentSatellite != nullptr) {
        currentSatellite->pushSatelliteXyz(CurrentLog->xCoordinate, CurrentLog->yCoordinate, CurrentLog->zCoordinate, CurrentHeader->week, CurrentHeader->milliseconds);
    }
}

void
SatelliteGroup::writePSRPOS(LogRawHeader* CurrentHeader, LogRawBodyPSRPOS* CurrentLog) {
    if (!isPsrPosSet_) {
        //we need only first data because it's not use for any kind of calculcation
        receiverLatitude_ = CurrentLog->latitude;
        receiverLongitude_ = CurrentLog->longitude;
        receiverHeightAboveSea_ = CurrentLog->heightAboveSea;

        isPsrPosSet_ = true;
    }
}

void
SatelliteGroup::writePSRXYZ(LogRawHeader* CurrentHeader, LogRawBodyPSRXYZ* CurrentLog) {
    //not usefull because coordinates is static in GlobalConfiguration
    if (!GlobalConfiguration::getInstance().getIsReceiverXyzSet()) {
        GlobalConfiguration::getInstance().setReceiverXYZ(CurrentLog->xCoordinate, CurrentLog->yCoordinate, CurrentLog->zCoordinate);
    }
}

void
SatelliteGroup::writeRANGE(LogRawHeader* CurrentHeader, LogRawBodyRANGE* CurrentLog) {
    ISatellite* currentSatellite = addSatellite(CurrentLog->satellitePRN, CurrentLog->channelTrackingStatus.satelliteSystem, CurrentLog->glonassFrequency, true);
    if (currentSatellite != nullptr) {
        SatelliteChannelType currentChannelType = getChannelType(currentSatellite->getSatelliteSystem(), CurrentLog->channelTrackingStatus.signalType, true);
        currentSatellite->pushCarrierToNoiseAndLockTime(currentChannelType, CurrentLog->carrierToNoise, CurrentLog->lockTime, CurrentHeader->week, CurrentHeader->milliseconds);
        currentSatellite->pushPsrPhaseLockTime(currentChannelType, CurrentLog->pseudorange, CurrentLog->carrierPhase, CurrentLog->lockTime, CurrentLog->carrierToNoise, CurrentHeader->week, CurrentHeader->milliseconds);
    }
}

void
SatelliteGroup::writeSATVIS2(LogRawHeader* CurrentHeader, LogBodySATVIS2* CurrentLog) {
    ISatellite* currentSatellite = addSatellite(CurrentLog->satelliteID, CurrentLog->satelliteSystem, CurrentLog->glonassFrequency, false);
    if (currentSatellite != nullptr) {
        currentSatellite->pushSatVisParams(CurrentLog->elevationAngle, CurrentLog->azimuthAngle, CurrentHeader->week, CurrentHeader->milliseconds);
    }
}

void
SatelliteGroup::writeISMDETOBS(LogRawHeader* CurrentHeader, LogBodyISMDETOBS* CurrentLog) {
    ISatellite* currentSatellite = addSatellite(CurrentLog->satellitePRN, CurrentLog->satelliteSystem, CurrentLog->glonassFrequency, false);

    if (currentSatellite != nullptr) {
        SatelliteChannelType currentChannelType = getChannelType(currentSatellite->getSatelliteSystem(), CurrentLog->signalType, false);
        for (size_t i = 0; i < 50; ++i) {
            currentSatellite->pushDetrendedPhase(currentChannelType, CurrentLog->ismData[i].accumulatedDopplerRange, CurrentHeader->week, CurrentHeader->milliseconds + CurrentLog->ismData[i].deltaMilliseconds);
            currentSatellite->pushDetrendedPower(currentChannelType, CurrentLog->ismData[i].power, CurrentHeader->week, CurrentHeader->milliseconds + CurrentLog->ismData[i].deltaMilliseconds);
        }
    }
}

void
SatelliteGroup::writeISMREDOBS(LogRawHeader* CurrentHeader, LogRawBodyISMREDOBS* CurrentLog) {
    ISatellite* currentSatellite = addSatellite(CurrentLog->satellitePRN, CurrentLog->satelliteSystem, CurrentLog->glonassFrequency, false);

    if (currentSatellite != nullptr) {
        SatelliteChannelType currentChannelType = getChannelType(currentSatellite->getSatelliteSystem(), CurrentLog->signalType, false);

        currentSatellite->pushNovatelS4(currentChannelType, CurrentLog->totalS4, CurrentHeader->week, CurrentHeader->milliseconds);
        currentSatellite->pushNovatelSigmaPhi60(currentChannelType, CurrentLog->phaseSigmaPhi60, CurrentHeader->week, CurrentHeader->milliseconds);
        currentSatellite->pushNovatelCodeCarrierDivergence(currentChannelType, CurrentLog->averageCMC, CurrentHeader->week, CurrentHeader->milliseconds);
    }
}

ISatellite*
SatelliteGroup::addSatellite(unsigned char satelliteId, unsigned char satelliteSystem, char glonassFrequency, bool isRangeSource) {
    boost::lock_guard<boost::mutex> lock(mutexAddSatellite_);

    SatelliteSystem newSatSystem = getSatelliteSystemType(satelliteSystem);
    int newSatelliteId = getSatelliteId(newSatSystem, satelliteId, isRangeSource);

    ISatellite* newSatellite = nullptr;
    switch (newSatSystem) {
        case SatelliteSystem::GPS:
        {
            try {
                newSatellite = satellites_.at(newSatelliteId);
            } catch (std::out_of_range& ex) {
                newSatellite = new GpsSatellite(newSatelliteId);
                satellites_.insert(std::make_pair(newSatelliteId, newSatellite));
            }

            break;
        }
        case SatelliteSystem::GLONASS:
        {
            char newGlonassFrequency = GlonassSatellite::getGlonassFrequency(glonassFrequency, isRangeSource);

            try {
                newSatellite = satellites_.at(newSatelliteId + 1000); //just a bit deduplicate magic
            } catch (std::out_of_range& ex) {
                newSatellite = new GlonassSatellite(newSatelliteId, newGlonassFrequency);
                satellites_.insert(std::make_pair(newSatelliteId + 1000, newSatellite));
            }

            break;
        }
        default:
            newSatellite = nullptr;
    }

    return newSatellite;
}

void
SatelliteGroup::forceCloseOutDataFiles(GnssLogTime& time) {
    for (auto& it : satellites_) {
        it.second->forceCloseOutDataFiles(time);
    }
}