/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   DoubleChannelCalculations.hpp
 * Author: dmitri
 */

#ifndef DOUBLECHANNELCALCULATIONS_HPP
#define DOUBLECHANNELCALCULATIONS_HPP

#include <fstream>
#include <string>
#include <atomic>

#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>

#include "ISatellite.hpp"
#include "GnssDataMeasurements2.hpp"
#include "GlobalConfiguration.hpp"
#include "IDoubleChannelCalculation.hpp"
#include "PashintsevCalculation.hpp"
#include "PlotData.hpp"

class DoubleChannelCalculations : public IDoubleChannelCalculation {
public:
    explicit DoubleChannelCalculations(ISatellite* satellite);
    virtual ~DoubleChannelCalculations();

    virtual void pushPsrPhaseLockTime1(double pseudorange, double carrierPhase, double lockTime, double carrierToNoise, unsigned short week, unsigned int milliseconds) override;
    virtual void pushPsrPhaseLockTime2(double pseudorange, double carrierPhase, double lockTime, double carrierToNoise, unsigned short week, unsigned int milliseconds) override;
    virtual void pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) override;

    virtual void stopThreads() override;

    virtual void closePlotData() override;

    virtual void forceCloseOutDataFiles(GnssLogTime& time) override;

    virtual void mergeData() override;
    virtual void calculateData() override;
private:
    struct DataForDoubleChannelCalculation {
        unsigned short week;
        unsigned int milliseconds;
        double pseudorange1;
        double carrierPhase1;
        double lockTime1;
        double carrierToNoise1;
        double pseudorange2;
        double carrierPhase2;
        double lockTime2;
        double carrierToNoise2;
        double elevationAngle;
        bool isProcessed;
    };

    struct ChannelData {
        unsigned short week;
        unsigned int milliseconds;
        double pseudorange;
        double carrierPhase;
        double lockTime;
        double carrierToNoise;
    };

    struct ElevationAngle {
        unsigned short week;
        unsigned int milliseconds;
        double elevationAngle;
    };

    GnssDataMeasurements2<ChannelData> firstChannel_;
    GnssDataMeasurements2<ChannelData> secondChannel_;
    GnssDataMeasurements2<ElevationAngle> elevationAngle_;
    GnssDataMeasurements2<DataForDoubleChannelCalculation> dataForCalculation_;

    PlotData* tecValuesPlotData_;

    ISatellite* parentSatellite_;

    std::atomic<bool> isStopThreads_;

    boost::mutex calcLock_;
    boost::mutex mergeLock_;

    pashintsev::PashintsevCalculation* pashintsevCalculation_;

    double frequency1_;
    double frequency2_;
    double waveLength1_;
    double waveLength2_;
    double tecCoefficient_;
    short glonassFrequency_;
};

#endif /* DOUBLECHANNELCALCULATIONS_HPP */

