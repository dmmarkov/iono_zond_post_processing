/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   PashintsevCalculation.hpp
 * Author: dmitri
 */

#ifndef PASHINTSEVCALCULATION_HPP
#define PASHINTSEVCALCULATION_HPP

#include <atomic>

#include <boost/thread/thread.hpp>

#include "ISatellite.hpp"
#include "PlotData.hpp"
#include "pashintsev_const.hpp"
#include "PashitsevSatelliteChannel.hpp"
#include "S4IrregulationsSearch.hpp"

namespace pashintsev {
    class PashintsevCalculation;
}


class pashintsev::PashintsevCalculation {
public:
    explicit PashintsevCalculation(ISatellite* satellite);
    virtual ~PashintsevCalculation();

    void pushTecWithoutAmbiguity(double tec, unsigned short week, unsigned int milliseconds);

    void stopThreads();
    void closePlotData();

    void forceCloseOutDataFiles(GnssLogTime& time);
private:
    struct DataForPashintsevCalculation {
        unsigned short week;
        unsigned int milliseconds;
        double value;
        bool isProcessed;
    };

    ISatellite* parentSatellite_;
    pashintsev::PashitsevSatelliteChannel* pashitsevChannel;
    S4IrregulationsSearch* s4irregulations;

    std::atomic<bool> isStopThreads_;
    size_t countOfFirstStep_;
    size_t countOfSecondStep_;

    double sumForStandardDeviation_;
    double sumForStandardDeviation2_;

    boost::thread firstStepCalculateData_; //Simple Moving Average over one minute
    boost::thread secondStepCalculateData_; //Simple Moving Average over 10 seconds
    boost::thread thirdStepCalculateData_; //Pashintsev SKO and other one step formula calculation
#ifdef ENABLE_PASHINTSEV_CALCULATIONS
    typedef typename GnssDataMeasurements<DataForPashintsevCalculation>::GnssIterator tecIt;

    GnssDataMeasurements<DataForPashintsevCalculation> listOfTec_;
    GnssDataMeasurements<DataForPashintsevCalculation> listOfDifferenceFirstStepSmoothAndTec_;
    GnssDataMeasurements<DataForPashintsevCalculation> listOfDifferenceSecondStepSmoothAndTec_;
#endif

    //set of lists for plotting data
    PlotData* tecStandardDeviationPlotData_;
    PlotData* sigmaPhiPlotData_;
    PlotData* s4PlotData_;
    PlotData* riceParamPlotData_;
    PlotData* probabilityOfErrorPlotData_;
    PlotData* nakagamiMPlotData_;
    PlotData* nErrPlotData_;

    //this list will be used for plotting, but not for calculations
    PlotData* differenceFirstStepPlotData_;
    PlotData* differenceSecondStepPlotData_;

    void firstStepCalculateData();
    void secondStepCalculateData();
    void thirdStepCalculateData();
    void smoothDataBySimpleMovingAverage(
#ifdef ENABLE_PASHINTSEV_CALCULATIONS
                                         GnssDataMeasurements<DataForPashintsevCalculation>& sourceList,
                                         GnssDataMeasurements<DataForPashintsevCalculation>& destList,
                                         PlotData* plotList,
                                         size_t& countOfProcessed,
                                         const size_t windowSize
#endif
    );

    PashintsevCalculation(const PashintsevCalculation& orig) = delete;
    PashintsevCalculation& operator=(const PashintsevCalculation& ) = delete;

};

#endif /* PASHINTSEVCALCULATION_HPP */

