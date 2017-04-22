/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   PashintsevCalculation.cpp
 * Author: dmitri
 */

#include <thread>
#include <chrono>

#include "PashintsevCalculation.hpp"
#include "gnss_const.hpp"
#include "project_defines.hpp"
#include "common_functions.hpp"
#include "PlotData.hpp"
#include "Logger.hpp"

#define ENABLE_PASHINTSEV_CALCULATIONS_

pashintsev::PashintsevCalculation::PashintsevCalculation(ISatellite* satellite) : isStopThreads_(false) {
    parentSatellite_ = satellite;

    countOfFirstStep_ = 0;
    countOfSecondStep_ = 0;
    sumForStandardDeviation_ = 0;
    sumForStandardDeviation2_ = 0;

#ifdef ENABLE_PASHINTSEV_CALCULATIONS

    pashitsevChannel = new pashintsev::PashitsevSatelliteChannel(satellite);
    s4irregulations = new S4IrregulationsSearch(pashitsevChannel);

    tecStandardDeviationPlotData_ = new PlotData(parentSatellite_->getSatelliteName(), "PTecStandardDeviation");
    tecStandardDeviationPlotData_->disableOutput();
    sigmaPhiPlotData_ = new PlotData(parentSatellite_->getSatelliteName(), "PSigmaPhi");
    sigmaPhiPlotData_->disableOutput();
    s4PlotData_ = new PlotData(parentSatellite_->getSatelliteName(), "PS4");
    s4PlotData_->disableOutput();
    riceParamPlotData_ = new PlotData(parentSatellite_->getSatelliteName(), "PRiceParam");
    riceParamPlotData_->disableOutput();
    probabilityOfErrorPlotData_ = new PlotData(parentSatellite_->getSatelliteName(), "PProbabilityOfError");
    probabilityOfErrorPlotData_->disableOutput();
    nakagamiMPlotData_ = new PlotData(parentSatellite_->getSatelliteName(), "PNakagamiM");
    nakagamiMPlotData_->disableOutput();
    nErrPlotData_ = new PlotData(parentSatellite_->getSatelliteName(), "PNErr");
    nErrPlotData_->disableOutput();
    differenceFirstStepPlotData_ = new PlotData(parentSatellite_->getSatelliteName(), "PTecDifferenceByFirstStep");
    differenceFirstStepPlotData_->disableOutput();
    differenceSecondStepPlotData_ = new PlotData(parentSatellite_->getSatelliteName(), "PTecDifferenceBySecondStep");
    differenceSecondStepPlotData_->disableOutput();

    firstStepCalculateData_ = boost::thread(boost::bind(&PashintsevCalculation::firstStepCalculateData, this));
    secondStepCalculateData_ = boost::thread(boost::bind(&PashintsevCalculation::secondStepCalculateData, this));
    thirdStepCalculateData_ = boost::thread(boost::bind(&PashintsevCalculation::thirdStepCalculateData, this));
#endif
    Logger::getInstance().info("Create PashintsevCalculation for satellite " + parentSatellite_->getSatelliteName());
}

pashintsev::PashintsevCalculation::~PashintsevCalculation() {
#ifdef ENABLE_PASHINTSEV_CALCULATIONS
    firstStepCalculateData_.join();
    secondStepCalculateData_.join();
    thirdStepCalculateData_.join();

    delete tecStandardDeviationPlotData_;
    delete sigmaPhiPlotData_;
    delete s4PlotData_;
    delete riceParamPlotData_;
    delete probabilityOfErrorPlotData_;
    delete nakagamiMPlotData_;
    delete nErrPlotData_;
    delete differenceFirstStepPlotData_;
    delete differenceSecondStepPlotData_;

    s4irregulations->stopThreads();
    delete s4irregulations;
    delete pashitsevChannel;
#endif
    Logger::getInstance().info("Destroy PashintsevCalculation for satellite " + parentSatellite_->getSatelliteName());
}

void
pashintsev::PashintsevCalculation::stopThreads() {
    isStopThreads_ = true;
}

void
pashintsev::PashintsevCalculation::pushTecWithoutAmbiguity(double tec, unsigned short week, unsigned int milliseconds) {
#ifdef ENABLE_PASHINTSEV_CALCULATIONS
    listOfTec_.push_back(new DataForPashintsevCalculation{week, milliseconds, tec, false});
#endif
}

void
pashintsev::PashintsevCalculation::smoothDataBySimpleMovingAverage(
#ifdef ENABLE_PASHINTSEV_CALCULATIONS
                                                       GnssDataMeasurements<DataForPashintsevCalculation>& sourceList,
                                                       GnssDataMeasurements<DataForPashintsevCalculation>& destList,
                                                       PlotData* plotList,
                                                       size_t& countOfProcessed,
                                                       const size_t windowSize
#endif
) {
#ifdef ENABLE_PASHINTSEV_CALCULATIONS
    if (!sourceList.is_empty()) {
        size_t sizeOfSourceList = sourceList.size();
        //first step it-s Simple Moving Average over one minute or 3000+1 values
        tecIt firstElement = sourceList.begin();
        tecIt middleElement = sourceList.begin();
        tecIt lastElement = sourceList.begin();

        if (countOfProcessed == 0) {
            countOfProcessed = 1;
            //listOfFirstStepSmoothedTec_.push_back(new DataForPashintsevCalculation{(*firstElement)->week, (*firstElement)->milliseconds, (*firstElement)->value, false});
            destList.push_back(new DataForPashintsevCalculation{(*firstElement)->week, (*firstElement)->milliseconds, 0.0, false});
        } else {
            size_t positionOfFirstElement = 1;

            while ((firstElement != sourceList.end()) && ((*firstElement)->isProcessed)) {
                ++firstElement;
                ++middleElement;
                ++lastElement;

                positionOfFirstElement++;
            }

            size_t sizeOfCurrentWindow = 0;
            if (countOfProcessed > windowSize) {
                sizeOfCurrentWindow = windowSize;
            } else {
                sizeOfCurrentWindow = countOfProcessed;
            }

            bool isEnougthSizeOfSource = (positionOfFirstElement + 2 * sizeOfCurrentWindow + 1) < sizeOfSourceList;

            if (isEnougthSizeOfSource) {
                std::advance(middleElement, sizeOfCurrentWindow);
                std::advance(lastElement, sizeOfCurrentWindow);
                std::advance(lastElement, sizeOfCurrentWindow);

                if (firstElement != sourceList.end() && lastElement != sourceList.end()) {
                    double sumOfFirstStep = 0;

                    for (tecIt it = firstElement; (!isStopThreads_) && (it != lastElement); ++it) {
                        sumOfFirstStep += (*it)->value;
                    }

                    while ((!isStopThreads_) && isEnougthSizeOfSource && (sizeOfCurrentWindow < windowSize) && (lastElement != sourceList.end())) {
                        sumOfFirstStep += (*lastElement)->value;

                        double smoothedValue = sumOfFirstStep / (sizeOfCurrentWindow * 2 + 1);

                        countOfProcessed++;
                        double newValue = (*middleElement)->value - smoothedValue;
                        destList.push_back(new DataForPashintsevCalculation{(*middleElement)->week, (*middleElement)->milliseconds, newValue, false});

                        ++middleElement;
                        ++lastElement;
                        if (lastElement != sourceList.end()) {
                            sumOfFirstStep += (*lastElement)->value;
                        }
                        ++lastElement;

                        sizeOfCurrentWindow++;

                        isEnougthSizeOfSource = (positionOfFirstElement + 2 * sizeOfCurrentWindow + 1) < sizeOfSourceList;
                    }

                    bool isReadyForDeleteFromSource = false;
                    while ((!isStopThreads_) && (sizeOfCurrentWindow == windowSize) && (lastElement != sourceList.end()) && isEnougthSizeOfSource) {
                        sumOfFirstStep += (*lastElement)->value;
                        double smoothedValue = sumOfFirstStep / (sizeOfCurrentWindow * 2 + 1);

                        countOfProcessed++;
                        double newValue = (*middleElement)->value - smoothedValue;
                        destList.push_back(new DataForPashintsevCalculation{(*middleElement)->week, (*middleElement)->milliseconds, newValue, false});

                        sumOfFirstStep -= (*firstElement)->value;
                        (*firstElement)->isProcessed = true;

                        isReadyForDeleteFromSource = true;

                        ++firstElement;
                        positionOfFirstElement++;
                        ++middleElement;
                        ++lastElement;

                        isEnougthSizeOfSource = (positionOfFirstElement + 2 * sizeOfCurrentWindow + 1) < sizeOfSourceList;
                    }

                    if (isReadyForDeleteFromSource) {
                        sourceList.erase(sourceList.begin(), firstElement);
                    }
                }
            }
        }
    }
#endif
}

void
pashintsev::PashintsevCalculation::firstStepCalculateData() {
#ifdef ENABLE_PASHINTSEV_CALCULATIONS
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    while (!isStopThreads_) {
        listOfTec_.lock();

        smoothDataBySimpleMovingAverage(
                                        listOfTec_,
                                        listOfDifferenceFirstStepSmoothAndTec_,
                                        differenceFirstStepPlotData_,
                                        countOfFirstStep_,
                                        FIRST_STEP_WINDOW_SIZE
                                        );


        listOfTec_.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
#endif
}

void
pashintsev::PashintsevCalculation::secondStepCalculateData() {
#ifdef ENABLE_PASHINTSEV_CALCULATIONS
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    while (!isStopThreads_) {
        listOfDifferenceFirstStepSmoothAndTec_.lock();

        smoothDataBySimpleMovingAverage(
                                        listOfDifferenceFirstStepSmoothAndTec_,
                                        listOfDifferenceSecondStepSmoothAndTec_,
                                        differenceSecondStepPlotData_,
                                        countOfSecondStep_,
                                        SECOND_STEP_WINDOW_SIZE
                                        );

        listOfDifferenceFirstStepSmoothAndTec_.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
#endif
}

void
pashintsev::PashintsevCalculation::thirdStepCalculateData() {
#ifdef ENABLE_PASHINTSEV_CALCULATIONS
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    while (!isStopThreads_) {
        listOfDifferenceSecondStepSmoothAndTec_.lock();

        if (listOfDifferenceSecondStepSmoothAndTec_.size() > 0) {
            try {
                //find first that was not processed
                tecIt firstNonProcessedElement = listOfDifferenceSecondStepSmoothAndTec_.begin();
                tecIt firstProcessedElement = listOfDifferenceSecondStepSmoothAndTec_.begin();
                size_t sigmaPhiCounter = 0;
                while (
                        (firstNonProcessedElement != listOfDifferenceSecondStepSmoothAndTec_.end())
                        &&
                        (*firstNonProcessedElement)->isProcessed
                        ) {
                    ++firstNonProcessedElement;
                    ++sigmaPhiCounter;
                }

                while (firstNonProcessedElement != listOfDifferenceSecondStepSmoothAndTec_.end()) {
                    if (sigmaPhiCounter == STANDARD_DEVIATION_COUNTER) {
                        //small hack that give advantage of saving memory
                        double oldPhase = (*firstProcessedElement)->value;
                        sumForStandardDeviation_ -= oldPhase;
                        sumForStandardDeviation2_ -= pow(oldPhase, 2);

                        ++firstProcessedElement;

                        double newPhase = (*firstNonProcessedElement)->value;
                        sumForStandardDeviation_ += newPhase;
                        sumForStandardDeviation2_ += pow(newPhase, 2);

                        double sum_f = sumForStandardDeviation_ / STANDARD_DEVIATION_COUNTER;
                        sum_f *= sum_f;
                        double sum_f2 = sumForStandardDeviation2_ / STANDARD_DEVIATION_COUNTER;

                        double newTecStandardDeviation = pow(sum_f2 - sum_f, 0.5);

                        double newSigmaPhi = (80.8 * PI * newTecStandardDeviation * pow(10.0, 16.0)) / (SPEED_OF_LIGHT * CALCULATION_FREQUENCY);

                        double newS4 = sqrt(1.0 - exp(-2.0 * pow(newSigmaPhi, 2.0)));
                        s4irregulations->pushS4Value((*firstNonProcessedElement)->week, (*firstNonProcessedElement)->milliseconds, newS4);

                        double newRiceParam = 1.0 / expm1(newSigmaPhi * newSigmaPhi);

                        double newProbabilityOfError = ((newRiceParam + 1.0) / (CALCULATION_SIGNAL_TO_NOISE + 2.0 * (newRiceParam + 1.0))) * exp(-(newRiceParam * CALCULATION_SIGNAL_TO_NOISE) / (CALCULATION_SIGNAL_TO_NOISE + 2.0 * (newRiceParam + 1.0)));

                        double newNakagamiM = 1.0 / (newS4 * newS4);

                        double newNErr = 0.5 * pow((2.0 * newNakagamiM) / (2.0 * newNakagamiM + CALCULATION_SIGNAL_TO_NOISE), newNakagamiM);
                    } else {
                        double newPhase = (*firstNonProcessedElement)->value;
                        sumForStandardDeviation_ += newPhase;
                        sumForStandardDeviation2_ += pow(newPhase, 2);

                        sigmaPhiCounter++;
                    }

                    (*firstNonProcessedElement)->isProcessed = true;
                    ++firstNonProcessedElement;
                }

                listOfDifferenceSecondStepSmoothAndTec_.erase(listOfDifferenceSecondStepSmoothAndTec_.begin(), firstProcessedElement);
            } catch (std::exception& ex) {
                Logger::getInstance().error(std::string("PashintsevCalculation::thirdStepCalculateData() error: ") + ex.what());
            }
        }

        listOfDifferenceSecondStepSmoothAndTec_.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
#endif
}

void
pashintsev::PashintsevCalculation::closePlotData() {
#ifdef ENABLE_PASHINTSEV_CALCULATIONS
    tecStandardDeviationPlotData_->disableOutput();
    tecStandardDeviationPlotData_->closeFile();

    sigmaPhiPlotData_->disableOutput();
    sigmaPhiPlotData_->closeFile();

    s4PlotData_->disableOutput();
    s4PlotData_->closeFile();

    riceParamPlotData_->disableOutput();
    riceParamPlotData_->closeFile();

    probabilityOfErrorPlotData_->disableOutput();
    probabilityOfErrorPlotData_->closeFile();

    nakagamiMPlotData_->disableOutput();
    nakagamiMPlotData_->closeFile();

    nErrPlotData_->disableOutput();
    nErrPlotData_->closeFile();
#endif
}

void
pashintsev::PashintsevCalculation::forceCloseOutDataFiles(GnssLogTime& time) {
#ifdef ENABLE_PASHINTSEV_CALCULATIONS
    tecStandardDeviationPlotData_->forceCloseOutDataFiles(time);
    sigmaPhiPlotData_->forceCloseOutDataFiles(time);
    s4PlotData_->forceCloseOutDataFiles(time);
    riceParamPlotData_->forceCloseOutDataFiles(time);
    probabilityOfErrorPlotData_->forceCloseOutDataFiles(time);
    nakagamiMPlotData_->forceCloseOutDataFiles(time);
    nErrPlotData_->forceCloseOutDataFiles(time);
#endif
}