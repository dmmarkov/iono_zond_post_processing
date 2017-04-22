/*
 * File:   SatelliteChannel.cpp
 * Author: dmitri
 */

#include "gnss_const.hpp"
#include "Logger.hpp"
#include "SatelliteChannel.hpp"

SatelliteChannel::SatelliteChannel() : frequency_(0),
                                       waveLength_(0),
                                       channelType_(SatelliteChannelType::primary),
                                       parentSatellite_(nullptr),
                                       channelName_(""),
                                       fullChannelName_(""),
                                       s4Calculation_(nullptr),
                                       sigmaPhiCalculation_(nullptr),
                                       elevationAnglePlotData_(nullptr),
                                       carrierToNoisePlotData_(nullptr),
                                       lockTimePlotData_(nullptr),
                                       novatelS4PlotData_(nullptr),
                                       novatelSigmaPhi60PlotData_(nullptr),
                                       novatelCodeCarrierDivergencePlotData_(nullptr)

{

}

SatelliteChannel::~SatelliteChannel() {
    delete elevationAnglePlotData_;
    delete carrierToNoisePlotData_;
    delete lockTimePlotData_;
    delete novatelS4PlotData_;
    delete novatelSigmaPhi60PlotData_;
    delete novatelCodeCarrierDivergencePlotData_;
}

void
SatelliteChannel::stopThreads() {
    if (s4Calculation_ != nullptr) {
        IS4Calculation* point = s4Calculation_;
        point->stopThreads();
    }
    if (sigmaPhiCalculation_ != nullptr) {
        ISigmaPhiCalculation* point = sigmaPhiCalculation_;
        point->stopThreads();
    }
}

void
SatelliteChannel::pushDetrendedPower(double detrendedPower, unsigned short week, unsigned int milliseconds) {
    if (s4Calculation_ != nullptr) {
        IS4Calculation* point = s4Calculation_;
        point->pushDetrendedPower(detrendedPower, week, milliseconds);
    }
}

void
SatelliteChannel::pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) {
    if (s4Calculation_ != nullptr) {
        IS4Calculation* point = s4Calculation_;
        point->pushElevationAngle(elevationAngle, week, milliseconds);
    }
    if (sigmaPhiCalculation_ != nullptr) {
        ISigmaPhiCalculation* point = sigmaPhiCalculation_;
        point->pushElevationAngle(elevationAngle, week, milliseconds);
    }
}

void
SatelliteChannel::pushCarrierToNoiseAndLockTime(double carrierToNoise, double lockTime, unsigned short week, unsigned int milliseconds) {
    if (s4Calculation_ != nullptr) {
        IS4Calculation* point = s4Calculation_;
        point->pushCarrierToNoiseAndLockTime(carrierToNoise, lockTime, week, milliseconds);
    }
    if (sigmaPhiCalculation_ != nullptr) {
        //sigma needs only lock time without carrier to noise
        ISigmaPhiCalculation* point = sigmaPhiCalculation_;
        point->pushLockTime(lockTime, week, milliseconds);
    }
}

void
SatelliteChannel::pushDetrendedPhase(double detrendedPhase, unsigned short week, unsigned int milliseconds) {
    if (sigmaPhiCalculation_ != nullptr) {
        ISigmaPhiCalculation* point = sigmaPhiCalculation_;
        point->pushDetrendedAdr(detrendedPhase, week, milliseconds);
    }
}

void
SatelliteChannel::pushNovatelS4(double novatelS4, unsigned short week, unsigned int milliseconds) {
//    PrintPlots::getInstance().printPlotData(novatelS4PlotData_,
//                                            week,
//                                            milliseconds,
//                                            novatelS4);
}

void
SatelliteChannel::pushNovatelSigmaPhi60(double novatelSigmaPhi60, unsigned short week, unsigned int milliseconds) {
//    PrintPlots::getInstance().printPlotData(novatelSigmaPhi60PlotData_,
//                                            week,
//                                            milliseconds,
//                                            novatelSigmaPhi60);
}

void
SatelliteChannel::pushNovatelCodeCarrierDivergence(double novatelCodeCarrierDivergence, unsigned short week, unsigned int milliseconds) {
//    PrintPlots::getInstance().printPlotData(novatelCodeCarrierDivergencePlotData_,
//                                            week,
//                                            milliseconds,
//                                            novatelCodeCarrierDivergence);
}

std::string
SatelliteChannel::getSatelliteName() {
    return parentSatellite_->getSatelliteName();
}

std::string
SatelliteChannel::getChannelName() {
    return channelName_;
}

std::string
SatelliteChannel::getFullChannelName() {
    return fullChannelName_;
}

double
SatelliteChannel::getFrequency() {
    return frequency_;
}

double
SatelliteChannel::getWaveLength() {
    return waveLength_;
}

ISatellite*
SatelliteChannel::getSatellite() {
    return parentSatellite_;
}

void
SatelliteChannel::createPlotData() {
    elevationAnglePlotData_ = new PlotData(parentSatellite_->getSatelliteName(), channelName_, "Elevation angle");
    carrierToNoisePlotData_ = new PlotData(parentSatellite_->getSatelliteName(), channelName_, "Novatel Carrier to Noise");
    lockTimePlotData_ = new PlotData(parentSatellite_->getSatelliteName(), channelName_, "Novatel Lock time");
    novatelS4PlotData_ = new PlotData(parentSatellite_->getSatelliteName(), channelName_, "Novatel S4 index");
    novatelSigmaPhi60PlotData_ = new PlotData(parentSatellite_->getSatelliteName(), channelName_, "Novatel SigmaPhi index");
    novatelCodeCarrierDivergencePlotData_ = new PlotData(parentSatellite_->getSatelliteName(), channelName_, "Novatel Code_Carrier Divergence");
}

void
SatelliteChannel::closePlotData() {
    if (s4Calculation_ != nullptr) {
        IS4Calculation* point = s4Calculation_;
        point->closePlotData();
    }
    if (sigmaPhiCalculation_ != nullptr) {
        ISigmaPhiCalculation* point = sigmaPhiCalculation_;
        point->closePlotData();
    }

    elevationAnglePlotData_->disableOutput();
    elevationAnglePlotData_->closeFile();

    carrierToNoisePlotData_->disableOutput();
    carrierToNoisePlotData_->closeFile();

    lockTimePlotData_->disableOutput();
    lockTimePlotData_->closeFile();

    novatelS4PlotData_->disableOutput();
    novatelS4PlotData_->closeFile();

    novatelSigmaPhi60PlotData_->disableOutput();
    novatelSigmaPhi60PlotData_->closeFile();

    novatelCodeCarrierDivergencePlotData_->disableOutput();
    novatelCodeCarrierDivergencePlotData_->closeFile();
}

void
SatelliteChannel::forceCloseOutDataFiles(GnssLogTime& time) {
    if (s4Calculation_ != nullptr) {
        IS4Calculation* point = s4Calculation_;
        point->forceCloseOutDataFiles(time);
    }
    if (sigmaPhiCalculation_ != nullptr) {
        ISigmaPhiCalculation* point = sigmaPhiCalculation_;
        point->forceCloseOutDataFiles(time);
    }

    elevationAnglePlotData_->forceCloseOutDataFiles(time);
    carrierToNoisePlotData_->forceCloseOutDataFiles(time);
    lockTimePlotData_->forceCloseOutDataFiles(time);
    novatelS4PlotData_->forceCloseOutDataFiles(time);
    novatelSigmaPhi60PlotData_->forceCloseOutDataFiles(time);
    novatelCodeCarrierDivergencePlotData_->forceCloseOutDataFiles(time);
}