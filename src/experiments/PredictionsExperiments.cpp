/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   PredictionsExperiments.cpp
 * Author: dmitri
 *
 * Created on 26 февраля 2017 г., 22:04
 */

#include "PredictionsExperiments.hpp"
#include "server_consts.hpp"
#include "ThreadManager.hpp"
#include "gnss_const.hpp"
#include "gnss_functions.hpp"
#include "GlonassSatellite.hpp"

static size_t numberOfElementsInExperiment2 = 3000;
static size_t numberOfElementsInExperiment3 = 3000;
static size_t numberOfElementsInExperiment4 = 3000;
static size_t numberOfElementsInExperiment6 = 3000;
static size_t numberOfElementsInExperiment7 = 3000;
static size_t numberOfElementsInExperiment8 = 3000;
static size_t numberOfElementsInExperiment9 = 3000;
static size_t numberOfElementsInExperiment10 = 3000;
static size_t numberOfElementsInExperiment11 = 3000;
static size_t numberOfElementsInExperiment12 = 3000;

PredictionsExperiments::PredictionsExperiments(ISatellite* satellite,
                                               unsigned short week,
                                               unsigned int milliseconds,
                                               int experimentId,
                                               int filterSatelliteSystemId,
                                               int filterSatelliteId,
                                               int filterChannel) :
    isStopThreads_(false),
    s4ValuesL1(400000),
    s4ValuesL2(400000),
    s4WithoutNoiseValuesL1(400000),
    s4WithoutNoiseValuesL2(400000),
    sigmaPhiValuesL1(400000),
    sigmaPhiValuesL2(400000),
    ElevationAngleValues(400000),
    ValuesForTecValues(400000)
{
    satellite_ = satellite;

    std::string satelliteSystemName = "";
    satelliteSystemId_ = 0;
    glonassFrequency_ = 0;

    double frequencyL1 = 0;
    double frequencyL2 = 0;

    std::stringstream filename;

    switch (satellite_->getSatelliteSystem()) {
        case SatelliteSystem::GPS:
            satelliteSystemName = "GPS";
            satelliteSystemId_ = 0;
            glonassFrequency_ = 0;
            frequencyL1 = GPS_L1_FREQUENCY;
            frequencyL2 = GPS_L2_FREQUENCY;

            filename << satelliteSystemName;
            filename << satellite->getSatelliteId();
            break;
        case SatelliteSystem::GLONASS:
            satelliteSystemName = "GLONASS";
            satelliteSystemId_ = 1;
            glonassFrequency_ = dynamic_cast<GlonassSatellite*> (satellite_)->getGlonassFrequency();
            frequencyL1 = GLO_L1_BASE_FREQUECY + GLO_L1_DELTA_FREQUECY * glonassFrequency_;
            frequencyL2 = GLO_L2_BASE_FREQUECY + GLO_L2_DELTA_FREQUECY * glonassFrequency_;

            filename << satelliteSystemName;
            filename << satellite->getSatelliteId() + GLONASS_SATELLITE_RANGE_CONST;
            break;
        default:
            satelliteSystemName = "Other";
            satelliteSystemId_ = 2;

            filename << satelliteSystemName;
            filename << satellite->getSatelliteId();
            break;
    }

    filename << "_" << week << "_" << milliseconds;

    tmpFileName_ = std::string(PATH_OUT_TMP_MERGE_DATA) + "/" + filename.str();
    outFileName_ = std::string(PATH_OUT_MERGE_DATA) + "/" + filename.str();

    outFile_.open(tmpFileName_.c_str(), std::ios_base::out);

    experiment16L1_ = nullptr;
    experiment16L2_ = nullptr;
    experiment17L1_ = nullptr;
    experiment17L2_ = nullptr;

    experimentId_ = experimentId;
    filterSatelliteSystemId_ = filterSatelliteSystemId;
    filterSatelliteId_ = filterSatelliteId;
    filterSatelliteChannelId_ = filterChannel;

//    experiment18GPSL1_ = { 0 };
//    experiment18GLOL1_ = { 0 };
//    experiment18GLOL2_ = { 0 };

    if (experimentId_ == 16) {
        experiment16L1_ = new IonoZondExperiment16(satelliteSystemId_, satellite_->getSatelliteId(), frequencyL1);
        experiment16L2_ = new IonoZondExperiment16(satelliteSystemId_, satellite_->getSatelliteId(), frequencyL2);
    }

    if (experimentId_ == 17) {
        experiment17L1_ = new IonoZondExperiment17(satelliteSystemId_, satellite_->getSatelliteId(), frequencyL1);
        experiment17L2_ = new IonoZondExperiment17(satelliteSystemId_, satellite_->getSatelliteId(), frequencyL2);
    }

    if (experimentId_ == 19) {
        experiment19L1_ = new IonoZondExperiment19(satellite, "L1");
        experiment19L2_ = new IonoZondExperiment19(satellite, "L2");
    }

    if (experimentId_ == 20) {
        experiment20_ = new IonoZondExperiment20(satellite);
    }


    ThreadManager::getInstance().addNewCalculation(this);
}

PredictionsExperiments::~PredictionsExperiments() {
//    ThreadManager::getInstance().releaseCalculation(this);

    if (experiment16L1_ != nullptr) {
        experiment16L1_->stopThreads();
        delete experiment16L1_;
    }

    if (experiment16L2_ != nullptr) {
        experiment16L2_->stopThreads();
        delete experiment16L2_;
    }

    if (experiment17L1_ != nullptr) {
        experiment17L1_->stopThreads();
        delete experiment17L1_;
    }

    if (experiment17L2_ != nullptr) {
        experiment17L2_->stopThreads();
        delete experiment17L2_;
    }

    std::cerr << "mv -v " << tmpFileName_ << " " << outFileName_ << " result = " << rename(tmpFileName_.c_str(), outFileName_.c_str()) << std::endl;

    if (experimentId_ == 18) {
        //print results
        if (satelliteSystemId_ == 0) {
            std::stringstream str;
            str << "GPS " << satellite_->getSatelliteId() << " L1";
            for (auto &t : experiment18GPSL1_.counters) {
                str << " " << t;
            }

            printCout(str.rdbuf());
        }

        if (satelliteSystemId_ == 1) {
            std::stringstream str;
            str << "GLONASS " << satellite_->getSatelliteId() << " L1";
            for (auto &t : experiment18GLOL1_.counters) {
                str << " " << t;
            }

            printCout(str.rdbuf());
        }

        if (satelliteSystemId_ == 1) {
            std::stringstream str;
            str << "GLONASS " << satellite_->getSatelliteId() << " L2";
            for (auto &t : experiment18GLOL2_.counters) {
                str << " " << t;
            }

            printCout(str.rdbuf());
        }
    }

    if (experimentId_ == 19) {
        delete experiment19L1_;
        delete experiment19L2_;
    }


    if (experimentId_ == 20) {
        delete experiment20_;
    }
}

void
PredictionsExperiments::stopThreads() {
    isStopThreads_ = true;

    if (experiment16L1_ != nullptr) {
        experiment16L1_->stopThreads();
    }

    if (experiment16L2_ != nullptr) {
        experiment16L2_->stopThreads();
    }

    if (experiment17L1_ != nullptr) {
        experiment17L1_->stopThreads();
    }

    if (experiment17L2_ != nullptr) {
        experiment17L2_->stopThreads();
    }

    ThreadManager::getInstance().releaseCalculation(this);
}

void
PredictionsExperiments::closePlotData() {
    outFile_.close();
}

void
PredictionsExperiments::forceCloseOutDataFiles(GnssLogTime& time) {
    //dummy method
}

void
PredictionsExperiments::pushS4ValuesL1(double s4, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        S4Value el = {
            .s4 = s4,
            .week = week,
            .milliseconds = milliseconds
        };

        s4ValuesL1.push_back(el);
    }
}

void
PredictionsExperiments::pushS4ValuesL2(double s4, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        S4Value el = {
            .s4 = s4,
            .week = week,
            .milliseconds = milliseconds
        };

        s4ValuesL2.push_back(el);
    }
}

void
PredictionsExperiments::pushS4WithoutNoiseValuesL1(double s4, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        S4Value el = {
            .s4 = s4,
            .week = week,
            .milliseconds = milliseconds
        };

        s4WithoutNoiseValuesL1.push_back(el);
    }
}

void
PredictionsExperiments::pushS4WithoutNoiseValuesL2(double s4, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        S4Value el = {
            .s4 = s4,
            .week = week,
            .milliseconds = milliseconds
        };

        s4WithoutNoiseValuesL2.push_back(el);
    }
}

void
PredictionsExperiments::pushSigmaPhiValuesL1(double sigmaPhi, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        SigmaPhiValue el = {
            .sigmaPhi = sigmaPhi,
            .week = week,
            .milliseconds = milliseconds
        };

        sigmaPhiValuesL1.push_back(el);
    }
}

void
PredictionsExperiments::pushSigmaPhiValuesL2(double sigmaPhi, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        SigmaPhiValue el = {
            .sigmaPhi = sigmaPhi,
            .week = week,
            .milliseconds = milliseconds
        };

        sigmaPhiValuesL2.push_back(el);
    }
}

void
PredictionsExperiments::pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        for (unsigned int i = 0; i < 1000; i += 20) {
            ElevationAngleValue el = {
                .elevationAngle = elevationAngle,
                .week = week,
                .milliseconds = milliseconds + i
            };

            ElevationAngleValues.push_back(el);
        }
    }
}

void
PredictionsExperiments::pushValuesForTec(double PsrML1, double PsrML2,
                                         double PsrFL1, double PsrFL2,
                                         double carrierToNoiseL1, double carrierToNoiseL2,
                                         double frequencyL1, double frequencyL2,
                                         double waveLengthL1, double waveLengthL2,
                                         double tec,
                                         unsigned short week, unsigned int milliseconds) {
    if (!isStopThreads_) {
        ValuesForTec el = {
            .PsrML1 = PsrML1,
            .PsrML2 = PsrML2,
            .PsrFL1 = PsrFL1,
            .PsrFL2 = PsrFL2,
            .carrierToNoiseL1 = carrierToNoiseL1,
            .carrierToNoiseL2 = carrierToNoiseL2,
            .frequencyL1 = frequencyL1,
            .frequencyL2 = frequencyL2,
            .waveLengthL1 = waveLengthL1,
            .waveLengthL2 = waveLengthL2,
            .week = week,
            .milliseconds = milliseconds,
            .tec = tec
        };

        ValuesForTecValues.push_back(el);
    }
}

void
PredictionsExperiments::mergeData() {
    boost::unique_lock<boost::mutex> lockMethod(mergeLock_, boost::try_to_lock);

    if (lockMethod && !isStopThreads_) {
        s4ValuesL1.lock();
        s4ValuesL2.lock();
        ElevationAngleValues.lock();
        ValuesForTecValues.lock();
        sigmaPhiValuesL1.lock();
        sigmaPhiValuesL2.lock();
        s4WithoutNoiseValuesL1.lock();
        s4WithoutNoiseValuesL2.lock();

        auto it1 = s4ValuesL1.begin();
        auto it2 = s4ValuesL2.begin();
        auto it3 = ElevationAngleValues.begin();
        auto it4 = ValuesForTecValues.begin();
        auto it5 = sigmaPhiValuesL1.begin();
        auto it6 = sigmaPhiValuesL2.begin();
        auto it7 = s4WithoutNoiseValuesL1.begin();
        auto it8 = s4WithoutNoiseValuesL2.begin();

        auto s4ValuesL1End = s4ValuesL1.end();
        auto s4ValuesL2End = s4ValuesL2.end();
        auto elevationAngleEnd = ElevationAngleValues.end();
        auto valuesForTecEnd = ValuesForTecValues.end();
        auto sigmaPhiValuesL1End = sigmaPhiValuesL1.end();
        auto sigmaPhiValuesL2End = sigmaPhiValuesL2.end();
        auto s4WithoutNoiseValuesL1End = s4WithoutNoiseValuesL1.end();
        auto s4WithoutNoiseValuesL2End = s4WithoutNoiseValuesL2.end();

        bool isReadyForDelete = false;
    #ifdef DEBUG
        size_t was_merged = 0;
    #endif
        while (true
                && it1 != s4ValuesL1End
                && it2 != s4ValuesL2End
                && it3 != elevationAngleEnd
                && it4 != valuesForTecEnd
                && it5 != sigmaPhiValuesL1End
                && it6 != sigmaPhiValuesL2End
                && it7 != s4WithoutNoiseValuesL1End
                && it8 != s4WithoutNoiseValuesL2End
                && !isStopThreads_) {

            long int it1Time = getMillisecondsTime((*it1).week, (*it1).milliseconds);
            long int it2Time = getMillisecondsTime((*it2).week, (*it2).milliseconds);
            long int it3Time = getMillisecondsTime((*it3).week, (*it3).milliseconds);
            long int it4Time = getMillisecondsTime((*it4).week, (*it4).milliseconds);
            long int it5Time = getMillisecondsTime((*it5).week, (*it5).milliseconds);
            long int it6Time = getMillisecondsTime((*it6).week, (*it6).milliseconds);
            long int it7Time = getMillisecondsTime((*it7).week, (*it7).milliseconds);
            long int it8Time = getMillisecondsTime((*it8).week, (*it8).milliseconds);

            if (it1Time < it2Time) {
                ++it1;
                continue;
            }

            if (it2Time < it1Time) {
                ++it2;
                continue;
            }

            if (it1Time < it3Time) {
                ++it1;
                ++it2;
                continue;
            }

            if (it1Time > it3Time) {
                ++it3;
                continue;
            }

            if (it1Time < it4Time) {
                ++it1;
                ++it2;
                ++it3;
                continue;
            }

            if (it1Time > it4Time) {
                ++it4;
                continue;
            }

            if (it1Time < it5Time) {
                ++it1;
                ++it2;
                ++it3;
                ++it4;
                continue;
            }

            if (it1Time > it5Time) {
                ++it5;
                continue;
            }

            if (it1Time < it6Time) {
                ++it1;
                ++it2;
                ++it3;
                ++it4;
                ++it5;
                continue;
            }

            if (it1Time > it6Time) {
                ++it6;
                continue;
            }

            if (it1Time < it7Time) {
                ++it1;
                ++it2;
                ++it3;
                ++it4;
                ++it5;
                ++it6;
                continue;
            }

            if (it1Time > it7Time) {
                ++it7;
                continue;
            }

            if (it1Time < it8Time) {
                ++it1;
                ++it2;
                ++it3;
                ++it4;
                ++it5;
                ++it6;
                ++it7;
                continue;
            }

            if (it1Time > it8Time) {
                ++it8;
                continue;
            }
    #ifdef DEBUG
            was_merged++;
    #endif

            if ((filterSatelliteSystemId_ == -1 || satelliteSystemId_ == filterSatelliteSystemId_)
                    && (filterSatelliteId_ == -1 || satellite_->getSatelliteId() == filterSatelliteId_)) {
                GnssLogTime* newTime = new GnssLogTime((*it1).week, (*it1).milliseconds);

                if (experimentId_ == 0) {
                    if ((filterSatelliteSystemId_ == -1 || satelliteSystemId_ == filterSatelliteSystemId_)
                            && (filterSatelliteId_ == -1 || satellite_->getSatelliteId() == filterSatelliteId_)
                            && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1)) {

                        outFile_ << newTime->getStringIsoTime() << ";" <<
                                newTime->getUtcTime().time_of_day().hours() << ";" <<
                                newTime->getUtcTime().time_of_day().minutes() << ";" <<
                                satelliteSystemId_ << ";" <<
                                satellite_->getSatelliteId() << ";" <<
                                glonassFrequency_ << ";" <<
                                doubleToString((*it1).s4, 10) << ";" <<
                                doubleToString((*it7).s4, 10) << ";" <<
                                doubleToString((*it2).s4, 10) << ";" <<
                                doubleToString((*it8).s4, 10) << ";" <<
                                doubleToString((*it5).sigmaPhi, 10) << ";" <<
                                doubleToString((*it6).sigmaPhi, 10) << ";" <<
                                doubleToString((*it3).elevationAngle, 5) << ";" <<
                                //тут должен быть азимут ещё
                                doubleToString((*it4).PsrML1, 10) << ";" <<
                                doubleToString((*it4).PsrML2, 10) << ";" <<
                                doubleToString(-1.0 * (*it4).PsrFL1 * (*it4).waveLengthL1, 10) << ";" <<
                                doubleToString(-1.0 * (*it4).PsrFL2 * (*it4).waveLengthL2, 10) << ";" <<
                                doubleToString((*it4).carrierToNoiseL1, 10) << ";" <<
                                doubleToString((*it4).carrierToNoiseL2, 10) << ";" <<
                                doubleToString((*it4).frequencyL1, 1) << ";" <<
                                doubleToString((*it4).frequencyL2, 1) << ";" <<
                                doubleToString((*it4).waveLengthL1, 10) << ";" <<
                                doubleToString((*it4).waveLengthL2, 10) << "\n";
                    }
                }

                if (experimentId_ == 1) {
                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        Experiment1LearnindData data;
                        data.s4 = (*it1).s4;
                        data.satelliteSystemId = satelliteSystemId_;
                        data.satelliteId = satellite_->getSatelliteId();
                        data.glonassFrequency = glonassFrequency_;
                        data.elevationAngle = (*it3).elevationAngle;
                        data.codePseudorande = (*it4).PsrML1;
                        data.phasePseudorande = -1.0 * (*it4).PsrFL1 * (*it4).waveLengthL1;
                        data.carrierToNoise = (*it4).carrierToNoiseL1;
                        data.frequency = (*it4).frequencyL1;
                        experiment1WriteData(data);
                    }
                }

                if (experimentId_ == 2) {
                    Experiment2LearnindData data;

                    data.time = *newTime;
                    data.satelliteSystemId = satelliteSystemId_;
                    data.satelliteId = satellite_->getSatelliteId();
                    data.elevationAngle = (*it3).elevationAngle;

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        data.frequency = (*it4).frequencyL1;
                        data.carrierToNoise = (*it4).carrierToNoiseL1;
                        data.codePseudorande = (*it4).PsrML1;

                        experiment2WriteData(&experiment2L1, data);
                    }

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2) {
                        data.frequency = (*it4).frequencyL2;
                        data.carrierToNoise = (*it4).carrierToNoiseL2;
                        data.codePseudorande = (*it4).PsrML2;

                        experiment2WriteData(&experiment2L2, data);
                    }
                }

                if (experimentId_ == 3) {
                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        Experiment3LearnindData data;

                        data.time = *newTime;
                        data.satelliteSystemId = satelliteSystemId_;
                        data.satelliteId = satellite_->getSatelliteId();
                        data.frequency = (*it4).frequencyL1;
                        data.carrierToNoiseL1 = (*it4).carrierToNoiseL1;
                        data.carrierToNoiseL2 = (*it4).carrierToNoiseL2;
                        data.s4 = (*it1).s4;
                        data.tecL1L2 = (*it4).tec;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment3WriteData(&experiment3L1, data);
                        }
                    }
                }

                if (experimentId_ == 4) {
                    Experiment4LearnindData data;

                    data.time = *newTime;
                    data.satelliteSystemId = satelliteSystemId_;
                    data.satelliteId = satellite_->getSatelliteId();
                    data.carrierToNoiseL1 = (*it4).carrierToNoiseL1;
                    data.carrierToNoiseL2 = (*it4).carrierToNoiseL2;
                    data.tecL1L2 = (*it4).tec;

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        //first channel
                        data.frequency = (*it4).frequencyL1;
                        data.sigmaPhi = (*it5).sigmaPhi;

                        if ((data.sigmaPhi > 0.0) && (data.sigmaPhi < 1.0)) {
                            experiment4WriteData(&experiment4L1, data);
                        }
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        //second channel
                        data.frequency = (*it4).frequencyL2;
                        data.sigmaPhi = (*it6).sigmaPhi;

                        if ((data.sigmaPhi > 0.0) && (data.sigmaPhi < 1.0)) {
                            experiment4WriteData(&experiment4L2, data);
                        }
                    }
                }

                if (experimentId_ == 5) {
                    Experiment5LearnindData data;

                    data.time = *newTime;
                    data.satelliteSystemId = satelliteSystemId_;
                    data.satelliteId = satellite_->getSatelliteId();

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        //first channel
                        data.frequency = (*it4).frequencyL1;
                        data.sigmaPhi = (*it5).sigmaPhi;
                        data.elevationAngle = (*it3).elevationAngle;
                        data.s4 = (*it1).s4;

                        if ((data.sigmaPhi > 0.0) && (data.sigmaPhi < 1.0) && (data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment5WriteData(data);
                        }
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        //second channel
                        data.frequency = (*it4).frequencyL2;
                        data.sigmaPhi = (*it6).sigmaPhi;
                        data.elevationAngle = (*it3).elevationAngle;
                        data.s4 = (*it2).s4;

                        if ((data.sigmaPhi > 0.0) && (data.sigmaPhi < 1.0) && (data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment5WriteData(data);
                        }
                    }
                }

                if (experimentId_ == 6) {
                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        Experiment6LearnindData data;

                        data.time = *newTime;
                        data.satelliteSystemId = satelliteSystemId_;
                        data.satelliteId = satellite_->getSatelliteId();
                        data.frequency = (*it4).frequencyL1;
                        data.carrierToNoiseL1 = (*it4).carrierToNoiseL1;
                        data.carrierToNoiseL2 = (*it4).carrierToNoiseL2;
                        data.s4 = (*it1).s4;
                        data.tecL1L2 = (*it4).tec;
                        data.elevationAngle = (*it3).elevationAngle;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment6WriteData(&experiment6, data);
                        }
                    }
                }

                if (experimentId_ == 7) {
                    Experiment7LearnindData data;

                    data.time = *newTime;
                    data.satelliteSystemId = satelliteSystemId_;
                    data.satelliteId = satellite_->getSatelliteId();
                    data.carrierToNoiseL1 = (*it4).carrierToNoiseL1;
                    data.carrierToNoiseL2 = (*it4).carrierToNoiseL2;
                    data.tecL1L2 = (*it4).tec;
                    data.elevationAngle = (*it3).elevationAngle;

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        //first channel
                        data.frequency = (*it4).frequencyL1;
                        data.sigmaPhi = (*it5).sigmaPhi;

                        if ((data.sigmaPhi > 0.0) && (data.sigmaPhi < 1.0)) {
                            experiment7WriteData(&experiment7L1, data);
                        }
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        //second channel
                        data.frequency = (*it4).frequencyL2;
                        data.sigmaPhi = (*it6).sigmaPhi;

                        if ((data.sigmaPhi > 0.0) && (data.sigmaPhi < 1.0)) {
                            experiment7WriteData(&experiment7L2, data);
                        }
                    }
                }

                if (experimentId_ == 8) {
                    Experiment8LearnindData data;

                    data.time = *newTime;
                    data.satelliteSystemId = satelliteSystemId_;
                    data.satelliteId = satellite_->getSatelliteId();
                    data.carrierToNoiseL1 = (*it4).carrierToNoiseL1;
                    data.carrierToNoiseL2 = (*it4).carrierToNoiseL2;
                    data.tecL1L2 = (*it4).tec;
                    data.elevationAngle = (*it3).elevationAngle;

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        //first channel
                        data.frequency = (*it4).frequencyL1;
                        data.s4 = (*it1).s4;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment8WriteData(&experiment8L1, data);
                        }
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        //second channel
                        data.frequency = (*it4).frequencyL2;
                        data.s4 = (*it2).s4;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment8WriteData(&experiment8L2, data);
                        }
                    }
                }

                if (experimentId_ == 9) {
                    Experiment9LearnindData data;

                    data.time = *newTime;
                    data.satelliteSystemId = satelliteSystemId_;
                    data.satelliteId = satellite_->getSatelliteId();
                    data.carrierToNoiseL1 = (*it4).carrierToNoiseL1;
                    data.carrierToNoiseL2 = (*it4).carrierToNoiseL2;
                    data.tecL1L2 = (*it4).tec;
                    data.elevationAngle = (*it3).elevationAngle;

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        //first channel
                        data.frequency = (*it4).frequencyL1;
                        data.sigmaPhi = (*it5).sigmaPhi;

                        if ((data.sigmaPhi > 0.0) && (data.sigmaPhi < 1.0)) {
                            experiment9WriteData(&experiment9L1, data);
                        }
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        //second channel
                        data.frequency = (*it4).frequencyL2;
                        data.sigmaPhi = (*it6).sigmaPhi;

                        if ((data.sigmaPhi > 0.0) && (data.sigmaPhi < 1.0)) {
                            experiment9WriteData(&experiment9L2, data);
                        }
                    }
                }

                if (experimentId_ == 10) {
                    Experiment10LearnindData data;

                    data.time = *newTime;
                    data.satelliteSystemId = satelliteSystemId_;
                    data.satelliteId = satellite_->getSatelliteId();
                    data.tecL1L2 = (*it4).tec;
                    data.elevationAngle = (*it3).elevationAngle;

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        //first channel
                        data.frequency = (*it4).frequencyL1;
                        data.sigmaPhi = (*it5).sigmaPhi;
                        data.carrierToNoise = (*it4).carrierToNoiseL1;

                        if ((data.sigmaPhi > 0.0) && (data.sigmaPhi < 1.0)) {
                            experiment10WriteData(&experiment10L1, data);
                        }
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        //second channel
                        data.frequency = (*it4).frequencyL2;
                        data.sigmaPhi = (*it6).sigmaPhi;
                        data.carrierToNoise = (*it4).carrierToNoiseL2;

                        if ((data.sigmaPhi > 0.0) && (data.sigmaPhi < 1.0)) {
                            experiment10WriteData(&experiment10L2, data);
                        }
                    }
                }

                if (experimentId_ == 11) {
                    Experiment11LearnindData data;

                    data.time = *newTime;
                    data.satelliteSystemId = satelliteSystemId_;
                    data.satelliteId = satellite_->getSatelliteId();
                    data.tecL1L2 = (*it4).tec;
                    data.elevationAngle = (*it3).elevationAngle;

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        //first channel
                        data.frequency = (*it4).frequencyL1;
                        data.s4 = (*it1).s4;
                        data.carrierToNoise = (*it4).carrierToNoiseL1;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment11WriteData(&experiment11L1, data);
                        }
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        //second channel
                        data.frequency = (*it4).frequencyL2;
                        data.s4 = (*it2).s4;
                        data.carrierToNoise = (*it4).carrierToNoiseL2;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment11WriteData(&experiment11L2, data);
                        }
                    }
                }

                if (experimentId_ == 12) {
                    Experiment12LearnindData data;

                    data.time = *newTime;
                    data.satelliteSystemId = satelliteSystemId_;
                    data.satelliteId = satellite_->getSatelliteId();
                    data.tecL1L2 = (*it4).tec;
                    data.elevationAngle = (*it3).elevationAngle;

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        //first channel
                        data.frequency = (*it4).frequencyL1;
                        data.sigmaPhi = (*it5).sigmaPhi;

                        if ((data.sigmaPhi > 0.0) && (data.sigmaPhi < 1.0)) {
                            experiment12WriteData(&experiment12L1, data);
                        }
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        //second channel
                        data.frequency = (*it4).frequencyL2;
                        data.sigmaPhi = (*it6).sigmaPhi;

                        if ((data.sigmaPhi > 0.0) && (data.sigmaPhi < 1.0)) {
                            experiment12WriteData(&experiment12L2, data);
                        }
                    }
                }

                if (experimentId_ == 13) {
                    Experiment13LearnindData data;

                    data.time = *newTime;
                    data.satelliteSystemId = satelliteSystemId_;
                    data.satelliteId = satellite_->getSatelliteId();
                    data.tecL1L2 = (*it4).tec;
                    data.elevationAngle = (*it3).elevationAngle;

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        //first channel
                        data.frequency = (*it4).frequencyL1;
                        data.s4 = (*it1).s4;
                        data.carrierToNoise = (*it4).carrierToNoiseL1;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment13WriteData(&experiment13L1, data);
                        }
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        //second channel
                        data.frequency = (*it4).frequencyL2;
                        data.s4 = (*it2).s4;
                        data.carrierToNoise = (*it4).carrierToNoiseL2;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment13WriteData(&experiment13L2, data);
                        }
                    }
                }

                if (experimentId_ == 14) {
                    Experiment14LearnindData data;

                    data.time = *newTime;
                    data.satelliteSystemId = satelliteSystemId_;
                    data.satelliteId = satellite_->getSatelliteId();
                    data.carrierToNoiseL1 = (*it4).carrierToNoiseL1;
                    data.carrierToNoiseL2 = (*it4).carrierToNoiseL2;
                    data.tecL1L2 = (*it4).tec;
                    data.elevationAngle = (*it3).elevationAngle;

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        //first channel
                        data.frequency = (*it4).frequencyL1;
                        data.s4 = (*it1).s4;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment14WriteData(&experiment14L1, data);
                        }
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        //second channel
                        data.frequency = (*it4).frequencyL2;
                        data.s4 = (*it2).s4;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment14WriteData(&experiment14L2, data);
                        }
                    }
                }

                if (experimentId_ == 15) {
                    Experiment15LearnindData data;

                    data.time = *newTime;
                    data.satelliteSystemId = satelliteSystemId_;
                    data.satelliteId = satellite_->getSatelliteId();
                    data.carrierToNoiseL1 = (*it4).carrierToNoiseL1;
                    data.carrierToNoiseL2 = (*it4).carrierToNoiseL2;
                    data.tecL1L2 = (*it4).tec;
                    data.elevationAngle = (*it3).elevationAngle;

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        //first channel
                        data.frequency = (*it4).frequencyL1;
                        data.s4 = (*it1).s4;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment15WriteData(&experiment15L1, data);
                        }
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        //second channel
                        data.frequency = (*it4).frequencyL2;
                        data.s4 = (*it2).s4;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment15WriteData(&experiment15L2, data);
                        }
                    }
                }

                if (experimentId_ == 16) {
                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        experiment16L1_->pushExperimentData(newTime, (*it1).s4);
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        experiment16L2_->pushExperimentData(newTime, (*it2).s4);
                    }
                }

                if (experimentId_ == 17) {
                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        experiment17L1_->pushExperimentData(newTime, (*it1).s4);
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        experiment17L2_->pushExperimentData(newTime, (*it2).s4);
                    }
                }

                if (experimentId_ == 18) {
                    Experiment18LearnindData data;

                    data.satelliteSystemId = satelliteSystemId_;
                    data.satelliteId = satellite_->getSatelliteId();

                    if (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 1) {
                        //first channel
                        data.frequency = (*it4).frequencyL1;
                        data.s4 = (*it1).s4;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            if (satelliteSystemId_ == 0) {
                                experiment18GPSL1WriteData(data);
                            }

                            if (satelliteSystemId_ == 1) {
                                experiment18GLOL1WriteData(data);
                            }
                        }
                    }

                    if (satelliteSystemId_ == 1 && (filterSatelliteChannelId_ == -1 || filterSatelliteChannelId_ == 2)) {
                        //second channel
                        data.frequency = (*it4).frequencyL2;
                        data.s4 = (*it2).s4;

                        if ((data.s4 > 0.0) && (data.s4 < 1.0)) {
                            experiment18GLOL2WriteData(data);
                        }
                    }
                }

                if (experimentId_ == 19) {
                    experiment19L1_->pushSigmaPhi((*it5).sigmaPhi);
                    experiment19L2_->pushSigmaPhi((*it6).sigmaPhi);
                }

                if (experimentId_ == 20) {
                    experiment20_->pushTec((*it4).tec);
                }

                delete newTime;
            }

            isReadyForDelete = true;

            ++it1;
            ++it2;
            ++it3;
            ++it4;
            ++it5;
            ++it6;
            ++it7;
            ++it8;
        }
    #ifdef DEBUG
        Logger::getInstance().info("SvmPredictionData::mergeAndOutData was_merged = " + intToString(was_merged) + " points");
    #endif
        if (isReadyForDelete) {
            s4ValuesL1.erase(s4ValuesL1.begin(), it1);
            s4ValuesL2.erase(s4ValuesL2.begin(), it2);
            ElevationAngleValues.erase(ElevationAngleValues.begin(), it3);
            ValuesForTecValues.erase(ValuesForTecValues.begin(), it4);
            sigmaPhiValuesL1.erase(sigmaPhiValuesL1.begin(), it5);
            sigmaPhiValuesL2.erase(sigmaPhiValuesL2.begin(), it6);
            s4WithoutNoiseValuesL1.erase(s4WithoutNoiseValuesL1.begin(), it7);
            s4WithoutNoiseValuesL2.erase(s4WithoutNoiseValuesL2.begin(), it8);
        } else {
            if (s4ValuesL1.percentOfFull() > 60) {
                Logger::getInstance().error("PredictionsExperiments. Data inconsistent. s4ValuesL1 full more than 60%");
                s4ValuesL1.erase(s4ValuesL1.begin(), s4ValuesL1.end());
            }

            if (s4ValuesL2.percentOfFull() > 60) {
                Logger::getInstance().error("PredictionsExperiments. Data inconsistent. s4ValuesL2 full more than 60%");
                s4ValuesL2.erase(s4ValuesL2.begin(), s4ValuesL2.end());
            }

            if (ElevationAngleValues.percentOfFull() > 60) {
                Logger::getInstance().error("PredictionsExperiments. Data inconsistent. ElevationAngleValues full more than 60%");
                ElevationAngleValues.erase(ElevationAngleValues.begin(), ElevationAngleValues.end());
            }

            if (ValuesForTecValues.percentOfFull() > 60) {
                Logger::getInstance().error("PredictionsExperiments. Data inconsistent. ValuesForTecValues full more than 60%");
                ValuesForTecValues.erase(ValuesForTecValues.begin(), ValuesForTecValues.end());
            }

            if (sigmaPhiValuesL1.percentOfFull() > 60) {
                Logger::getInstance().error("PredictionsExperiments. Data inconsistent. sigmaPhiValuesL1 full more than 60%");
                sigmaPhiValuesL1.erase(sigmaPhiValuesL1.begin(), sigmaPhiValuesL1.end());
            }

            if (sigmaPhiValuesL2.percentOfFull() > 60) {
                Logger::getInstance().error("PredictionsExperiments. Data inconsistent. sigmaPhiValuesL2 full more than 60%");
                sigmaPhiValuesL2.erase(sigmaPhiValuesL2.begin(), sigmaPhiValuesL2.end());
            }

            if (s4WithoutNoiseValuesL1.percentOfFull() > 60) {
                Logger::getInstance().error("PredictionsExperiments. Data inconsistent. s4WithoutNoiseValuesL1 full more than 60%");
                s4WithoutNoiseValuesL1.erase(s4WithoutNoiseValuesL1.begin(), s4WithoutNoiseValuesL1.end());
            }

            if (s4WithoutNoiseValuesL2.percentOfFull() > 60) {
                Logger::getInstance().error("PredictionsExperiments. Data inconsistent. s4WithoutNoiseValuesL2 full more than 60%");
                s4WithoutNoiseValuesL2.erase(s4WithoutNoiseValuesL2.begin(), s4WithoutNoiseValuesL2.end());
            }
        }

        s4WithoutNoiseValuesL2.unlock();
        s4WithoutNoiseValuesL1.unlock();
        sigmaPhiValuesL2.unlock();
        sigmaPhiValuesL1.unlock();
        ValuesForTecValues.unlock();
        ElevationAngleValues.unlock();
        s4ValuesL2.unlock();
        s4ValuesL1.unlock();
    }
}

void
PredictionsExperiments::calculateData() {
    //dummy method
}

void
PredictionsExperiments::experiment1WriteData(Experiment1LearnindData& data) {
    std::stringstream text;

    //only s4 l1

    text << data.s4 << " | ";
    text << "satellite_system_id:" << data.satelliteSystemId << " ";
    text << "frequency:" << doubleToString(data.frequency, 0) << " ";
    text << "elevation_angle:" << doubleToString(data.elevationAngle, 10) << " ";
    text << "code_pseudorange:" << doubleToString(data.codePseudorande, 10) << " ";
    text << "phase_pseudorande:" << doubleToString(data.phasePseudorande, 10) << " ";
    text << "carrier_to_noise:" << doubleToString(data.carrierToNoise, 10);

    printCout(text.rdbuf());
}

void
PredictionsExperiments::experiment2WriteData(Experiment2SatelliteChannelData *channel, Experiment2LearnindData& data) {
    if (!channel->listOfData.empty()) {
        boost::posix_time::time_duration timeDiff = data.time.getUtcTime() - channel->lastLogTime.getUtcTime();

        if (timeDiff.total_milliseconds() != 20) {
            channel->listOfData.clear();
        }

        if (channel->listOfData.size() == numberOfElementsInExperiment2) {
            std::stringstream text;

            text << doubleToString(data.codePseudorande, 10) << " |SatelliteAndChannel ";
            text << "satellite_system_id:" << data.satelliteSystemId << " ";
            text << "satellite_id:" << data.satelliteId << " ";
            text << "frequency:" << doubleToString(data.frequency, 0) << " ";

            size_t i = 1;
            std::stringstream pseudoranges;
            pseudoranges << "pseudoranges ";
            std::stringstream carrierToNoises;
            carrierToNoises << "carrier_to_noises ";
            std::stringstream elevationAngles;
            elevationAngles << "elevation_angles ";
            for (auto it1 = channel->listOfData.begin(); it1 != channel->listOfData.end(); ++it1) {
                pseudoranges << "pseudorange_" << i << ":" << doubleToString((*it1).codePseudorande, 10) << " ";
                carrierToNoises << "carrier_to_noise_" << i << ":" << doubleToString((*it1).carrierToNoise, 10) << " ";
                elevationAngles << "elevation_angle_" << i << ":" << doubleToString((*it1).elevationAngle, 10) << " ";

                ++i;
            }

            text << "|" << pseudoranges.str();
            text << "|" << carrierToNoises.str();
            text << "|" << elevationAngles.str();

            printCout(text.rdbuf());

            channel->listOfData.pop_front();
        }
    }

    channel->lastLogTime = data.time;

    Experiment2ChannelData newData;
    newData.elevationAngle = data.elevationAngle;
    newData.carrierToNoise = data.carrierToNoise;
    newData.codePseudorande = data.codePseudorande;

    channel->listOfData.push_back(newData);
}

void
PredictionsExperiments::experiment3WriteData(Experiment3SatelliteData *channel, Experiment3LearnindData& data) {
    if (!channel->listOfTec.empty()) {
        boost::posix_time::time_duration timeDiff = data.time.getUtcTime() - channel->lastLogTime.getUtcTime();

        if (timeDiff.total_milliseconds() != 20) {
            channel->listOfTec.clear();
            channel->listOfCarrierToNoiseL1.clear();
            channel->listOfCarrierToNoiseL2.clear();
            channel->sumOfTec = 0;
            channel->sumOfTec2 = 0;
            channel->sumOfCarrierToNoiseL1 = 0;
            channel->sumOfCarrierToNoiseL12 = 0;
            channel->sumOfCarrierToNoiseL2 = 0;
            channel->sumOfCarrierToNoiseL22 = 0;
        }

        if (channel->listOfTec.size() == numberOfElementsInExperiment3) {
            double sum_f = channel->sumOfTec / numberOfElementsInExperiment3;
            sum_f *= sum_f;
            double sum_f2 = channel->sumOfTec2 / numberOfElementsInExperiment3;
            double newSigmaOfTec = pow(sum_f2 - sum_f, 0.5);

            sum_f = channel->sumOfCarrierToNoiseL1 / numberOfElementsInExperiment3;
            sum_f *= sum_f;
            sum_f2 = channel->sumOfCarrierToNoiseL12 / numberOfElementsInExperiment3;
            double newSigmaOfCarrierToNoiseL1 = pow(sum_f2 - sum_f, 0.5);

            sum_f = channel->sumOfCarrierToNoiseL2 / numberOfElementsInExperiment3;
            sum_f *= sum_f;
            sum_f2 = channel->sumOfCarrierToNoiseL22 / numberOfElementsInExperiment3;
            double newSigmaOfCarrierToNoiseL2 = pow(sum_f2 - sum_f, 0.5);

            std::stringstream text;
            text << doubleToString(data.s4, 10) << " | ";
            text << "satellite_system_id:" << data.satelliteSystemId << " ";
            text << "satellite_id:" << data.satelliteId << " ";
            text << "frequency:" << doubleToString(data.frequency, 0) << " ";
            text << "std_dev_tec:" << doubleToString(newSigmaOfTec, 10) << " ";
            text << "std_dev_carrier_to_noise_l1:" << doubleToString(newSigmaOfCarrierToNoiseL1, 10) << " ";
            text << "std_dev_carrier_to_noise_l2:" << doubleToString(newSigmaOfCarrierToNoiseL2, 10);
            printCout(text.rdbuf());

            channel->sumOfTec -= channel->listOfTec.front();
            channel->sumOfTec2 -= pow(channel->listOfTec.front(), 2.0);

            channel->sumOfCarrierToNoiseL1 -= channel->listOfCarrierToNoiseL1.front();
            channel->sumOfCarrierToNoiseL12 -= pow(channel->listOfCarrierToNoiseL1.front(), 2.0);

            channel->sumOfCarrierToNoiseL2 -= channel->listOfCarrierToNoiseL2.front();
            channel->sumOfCarrierToNoiseL22 -= pow(channel->listOfCarrierToNoiseL2.front(), 2.0);

            channel->listOfTec.pop_front();
            channel->listOfCarrierToNoiseL1.pop_front();
            channel->listOfCarrierToNoiseL2.pop_front();
        }

        channel->sumOfTec += data.tecL1L2;
        channel->sumOfTec2 += pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoiseL1 += data.carrierToNoiseL1;
        channel->sumOfCarrierToNoiseL12 += pow(data.carrierToNoiseL1, 2);

        channel->sumOfCarrierToNoiseL2 += data.carrierToNoiseL2;
        channel->sumOfCarrierToNoiseL22 += pow(data.carrierToNoiseL2, 2);
    } else {
        channel->sumOfTec = data.tecL1L2;
        channel->sumOfTec2 = pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoiseL1 = data.carrierToNoiseL1;
        channel->sumOfCarrierToNoiseL12 = pow(data.carrierToNoiseL1, 2);

        channel->sumOfCarrierToNoiseL2 = data.carrierToNoiseL2;
        channel->sumOfCarrierToNoiseL22 = pow(data.carrierToNoiseL2, 2);
    }

    channel->lastLogTime = data.time;

    channel->listOfTec.push_back(data.tecL1L2);
    channel->listOfCarrierToNoiseL1.push_back(data.carrierToNoiseL1);
    channel->listOfCarrierToNoiseL2.push_back(data.carrierToNoiseL2);
}

void
PredictionsExperiments::experiment4WriteData(Experiment4SatelliteData *channel, Experiment4LearnindData& data) {
    if (!channel->listOfTec.empty()) {
        boost::posix_time::time_duration timeDiff = data.time.getUtcTime() - channel->lastLogTime.getUtcTime();

        if (timeDiff.total_milliseconds() != 20) {
            channel->listOfTec.clear();
            channel->listOfCarrierToNoiseL1.clear();
            channel->listOfCarrierToNoiseL2.clear();
            channel->sumOfTec = 0;
            channel->sumOfTec2 = 0;
            channel->sumOfCarrierToNoiseL1 = 0;
            channel->sumOfCarrierToNoiseL12 = 0;
            channel->sumOfCarrierToNoiseL2 = 0;
            channel->sumOfCarrierToNoiseL22 = 0;
        }

        if (channel->listOfTec.size() == numberOfElementsInExperiment4) {
            double sum_f = channel->sumOfTec / numberOfElementsInExperiment4;
            sum_f *= sum_f;
            double sum_f2 = channel->sumOfTec2 / numberOfElementsInExperiment4;
            double newSigmaOfTec = pow(sum_f2 - sum_f, 0.5);

            sum_f = channel->sumOfCarrierToNoiseL1 / numberOfElementsInExperiment4;
            sum_f *= sum_f;
            sum_f2 = channel->sumOfCarrierToNoiseL12 / numberOfElementsInExperiment4;
            double newSigmaOfCarrierToNoiseL1 = pow(sum_f2 - sum_f, 0.5);

            sum_f = channel->sumOfCarrierToNoiseL2 / numberOfElementsInExperiment4;
            sum_f *= sum_f;
            sum_f2 = channel->sumOfCarrierToNoiseL22 / numberOfElementsInExperiment4;
            double newSigmaOfCarrierToNoiseL2 = pow(sum_f2 - sum_f, 0.5);

            std::stringstream text;
            text << doubleToString(data.sigmaPhi, 10) << " | ";
            text << "satellite_system_id:" << data.satelliteSystemId << " ";
            text << "satellite_id:" << data.satelliteId << " ";
            text << "frequency:" << doubleToString(data.frequency, 0) << " ";
            text << "std_dev_tec:" << doubleToString(newSigmaOfTec, 10) << " ";
            text << "std_dev_carrier_to_noise_l1:" << doubleToString(newSigmaOfCarrierToNoiseL1, 10) << " ";
            text << "std_dev_carrier_to_noise_l2:" << doubleToString(newSigmaOfCarrierToNoiseL2, 10);
            printCout(text.rdbuf());

            channel->sumOfTec -= channel->listOfTec.front();
            channel->sumOfTec2 -= pow(channel->listOfTec.front(), 2.0);

            channel->sumOfCarrierToNoiseL1 -= channel->listOfCarrierToNoiseL1.front();
            channel->sumOfCarrierToNoiseL12 -= pow(channel->listOfCarrierToNoiseL1.front(), 2.0);

            channel->sumOfCarrierToNoiseL2 -= channel->listOfCarrierToNoiseL2.front();
            channel->sumOfCarrierToNoiseL22 -= pow(channel->listOfCarrierToNoiseL2.front(), 2.0);

            channel->listOfTec.pop_front();
            channel->listOfCarrierToNoiseL1.pop_front();
            channel->listOfCarrierToNoiseL2.pop_front();
        }

        channel->sumOfTec += data.tecL1L2;
        channel->sumOfTec2 += pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoiseL1 += data.carrierToNoiseL1;
        channel->sumOfCarrierToNoiseL12 += pow(data.carrierToNoiseL1, 2);

        channel->sumOfCarrierToNoiseL2 += data.carrierToNoiseL2;
        channel->sumOfCarrierToNoiseL22 += pow(data.carrierToNoiseL2, 2);
    } else {
        channel->sumOfTec = data.tecL1L2;
        channel->sumOfTec2 = pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoiseL1 = data.carrierToNoiseL1;
        channel->sumOfCarrierToNoiseL12 = pow(data.carrierToNoiseL1, 2);

        channel->sumOfCarrierToNoiseL2 = data.carrierToNoiseL2;
        channel->sumOfCarrierToNoiseL22 = pow(data.carrierToNoiseL2, 2);
    }

    channel->lastLogTime = data.time;
    channel->listOfTec.push_back(data.tecL1L2);
    channel->listOfCarrierToNoiseL1.push_back(data.carrierToNoiseL1);
    channel->listOfCarrierToNoiseL2.push_back(data.carrierToNoiseL2);
}

void
PredictionsExperiments::experiment5WriteData(Experiment5LearnindData& data) {
    std::stringstream text;
    text << doubleToString(data.s4, 10) << " | ";
    text << "satellite_system_id:" << data.satelliteSystemId << " ";
    text << "satellite_id:" << data.satelliteId << " ";
    text << "frequency:" << doubleToString(data.frequency, 0) << " ";
    text << "elevation_angle:" << doubleToString(data.elevationAngle, 0) << " ";
    text << "sigma_phi:" << doubleToString(data.sigmaPhi, 10);

    printCout(text.rdbuf());
}

void
PredictionsExperiments::experiment6WriteData(Experiment6SatelliteData *channel, Experiment6LearnindData& data) {
    if (!channel->listOfTec.empty()) {
        boost::posix_time::time_duration timeDiff = data.time.getUtcTime() - channel->lastLogTime.getUtcTime();

        if (timeDiff.total_milliseconds() != 20) {
            channel->listOfTec.clear();
            channel->sumOfTec = 0;
            channel->sumOfTec2 = 0;
        }

        if (channel->listOfTec.size() == numberOfElementsInExperiment6) {
            double sum_f = channel->sumOfTec / numberOfElementsInExperiment6;
            sum_f *= sum_f;
            double sum_f2 = channel->sumOfTec2 / numberOfElementsInExperiment6;
            double newSigmaOfTec = pow(sum_f2 - sum_f, 0.5);

            std::stringstream text;
            text << doubleToString(data.s4, 10) << " | ";
            text << "satellite_system_id:" << data.satelliteSystemId << " ";
            text << "satellite_id:" << data.satelliteId << " ";
            text << "frequency:" << doubleToString(data.frequency, 0) << " ";
            text << "std_dev_tec:" << doubleToString(newSigmaOfTec, 10) << " ";
            text << "carrier_to_noise_l1:" << doubleToString(data.carrierToNoiseL1, 10) << " ";
            text << "carrier_to_noise_l2:" << doubleToString(data.carrierToNoiseL2, 10) << " ";
            text << "elevation_angle:" << doubleToString(data.elevationAngle, 10);
            printCout(text.rdbuf());

            channel->sumOfTec -= channel->listOfTec.front();
            channel->sumOfTec2 -= pow(channel->listOfTec.front(), 2.0);

            channel->listOfTec.pop_front();
        }

        channel->sumOfTec += data.tecL1L2;
        channel->sumOfTec2 += pow(data.tecL1L2, 2);
    } else {
        channel->sumOfTec = data.tecL1L2;
        channel->sumOfTec2 = pow(data.tecL1L2, 2);
    }

    channel->lastLogTime = data.time;
    channel->listOfTec.push_back(data.tecL1L2);
}

void
PredictionsExperiments::experiment7WriteData(Experiment7SatelliteData *channel, Experiment7LearnindData& data) {
    if (!channel->listOfTec.empty() > 0) {
        boost::posix_time::time_duration timeDiff = data.time.getUtcTime() - channel->lastLogTime.getUtcTime();

        if (timeDiff.total_milliseconds() != 20) {
            channel->listOfTec.clear();
            channel->sumOfTec = 0;
            channel->sumOfTec2 = 0;
        }

        if (channel->listOfTec.size() == numberOfElementsInExperiment7) {
            double sum_f = channel->sumOfTec / numberOfElementsInExperiment7;
            sum_f *= sum_f;
            double sum_f2 = channel->sumOfTec2 / numberOfElementsInExperiment7;
            double newSigmaOfTec = pow(sum_f2 - sum_f, 0.5);

            std::stringstream text;
            text << doubleToString(data.sigmaPhi, 10) << " | ";
            text << "satellite_system_id:" << data.satelliteSystemId << " ";
            text << "satellite_id:" << data.satelliteId << " ";
            text << "frequency:" << doubleToString(data.frequency, 0) << " ";
            text << "std_dev_tec:" << doubleToString(newSigmaOfTec, 10) << " ";
            text << "carrier_to_noise_l1:" << doubleToString(data.carrierToNoiseL1, 10) << " ";
            text << "carrier_to_noise_l2:" << doubleToString(data.carrierToNoiseL2, 10) << " ";
            text << "elevation_angle:" << doubleToString(data.elevationAngle, 10);
            printCout(text.rdbuf());

            channel->sumOfTec -= channel->listOfTec.front();
            channel->sumOfTec2 -= pow(channel->listOfTec.front(), 2.0);

            channel->listOfTec.pop_front();
        }

        channel->sumOfTec += data.tecL1L2;
        channel->sumOfTec2 += pow(data.tecL1L2, 2);
    } else {
        channel->sumOfTec = data.tecL1L2;
        channel->sumOfTec2 = pow(data.tecL1L2, 2);
    }

    channel->lastLogTime = data.time;
    channel->listOfTec.push_back(data.tecL1L2);
}

void
PredictionsExperiments::experiment8WriteData(Experiment8SatelliteData *channel, Experiment8LearnindData& data) {
    if (!channel->listOfTec.empty()) {
        boost::posix_time::time_duration timeDiff = data.time.getUtcTime() - channel->lastLogTime.getUtcTime();

        if (timeDiff.total_milliseconds() != 20) {
            channel->listOfTec.clear();
            channel->listOfCarrierToNoiseL1.clear();
            channel->listOfCarrierToNoiseL2.clear();
            channel->sumOfTec = 0;
            channel->sumOfTec2 = 0;
            channel->sumOfCarrierToNoiseL1 = 0;
            channel->sumOfCarrierToNoiseL12 = 0;
            channel->sumOfCarrierToNoiseL2 = 0;
            channel->sumOfCarrierToNoiseL22 = 0;

            std::cerr << "Clear experiment data!\n" << "Previous timestamp " << channel->lastLogTime.getStringIsoTime() << "Current timestamp " << data.time.getStringIsoTime() << "\n";

        }

        if (channel->listOfTec.size() == numberOfElementsInExperiment8) {
            double sum_f = channel->sumOfTec / numberOfElementsInExperiment8;
            sum_f *= sum_f;
            double sum_f2 = channel->sumOfTec2 / numberOfElementsInExperiment8;
            double newSigmaOfTec = pow(sum_f2 - sum_f, 0.5);

            sum_f = channel->sumOfCarrierToNoiseL1 / numberOfElementsInExperiment8;
            sum_f *= sum_f;
            sum_f2 = channel->sumOfCarrierToNoiseL12 / numberOfElementsInExperiment8;
            double newSigmaOfCarrierToNoiseL1 = pow(sum_f2 - sum_f, 0.5);

            sum_f = channel->sumOfCarrierToNoiseL2 / numberOfElementsInExperiment8;
            sum_f *= sum_f;
            sum_f2 = channel->sumOfCarrierToNoiseL22 / numberOfElementsInExperiment8;
            double newSigmaOfCarrierToNoiseL2 = pow(sum_f2 - sum_f, 0.5);

            std::stringstream text;
            text << doubleToString(data.s4, 10) << " | ";
            text << "satellite_system_id:" << data.satelliteSystemId << " ";
            text << "satellite_id:" << data.satelliteId << " ";
            text << "frequency:" << doubleToString(data.frequency, 0) << " ";
            text << "std_dev_tec:" << doubleToString(newSigmaOfTec, 10) << " ";
            text << "std_dev_carrier_to_noise_l1:" << doubleToString(newSigmaOfCarrierToNoiseL1, 10) << " ";
            text << "std_dev_carrier_to_noise_l2:" << doubleToString(newSigmaOfCarrierToNoiseL2, 10) << " ";
            text << "elevation_angle:" << doubleToString(data.elevationAngle, 1);

            printCout(text.rdbuf());

            channel->sumOfTec -= channel->listOfTec.front();
            channel->sumOfTec2 -= pow(channel->listOfTec.front(), 2.0);

            channel->sumOfCarrierToNoiseL1 -= channel->listOfCarrierToNoiseL1.front();
            channel->sumOfCarrierToNoiseL12 -= pow(channel->listOfCarrierToNoiseL1.front(), 2.0);

            channel->sumOfCarrierToNoiseL2 -= channel->listOfCarrierToNoiseL2.front();
            channel->sumOfCarrierToNoiseL22 -= pow(channel->listOfCarrierToNoiseL2.front(), 2.0);

            channel->listOfTec.pop_front();
            channel->listOfCarrierToNoiseL1.pop_front();
            channel->listOfCarrierToNoiseL2.pop_front();
        }

        channel->sumOfTec += data.tecL1L2;
        channel->sumOfTec2 += pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoiseL1 += data.carrierToNoiseL1;
        channel->sumOfCarrierToNoiseL12 += pow(data.carrierToNoiseL1, 2);

        channel->sumOfCarrierToNoiseL2 += data.carrierToNoiseL2;
        channel->sumOfCarrierToNoiseL22 += pow(data.carrierToNoiseL2, 2);
    } else {
        channel->sumOfTec = data.tecL1L2;
        channel->sumOfTec2 = pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoiseL1 = data.carrierToNoiseL1;
        channel->sumOfCarrierToNoiseL12 = pow(data.carrierToNoiseL1, 2);

        channel->sumOfCarrierToNoiseL2 = data.carrierToNoiseL2;
        channel->sumOfCarrierToNoiseL22 = pow(data.carrierToNoiseL2, 2);
    }

    channel->lastLogTime = data.time;

    channel->listOfTec.push_back(data.tecL1L2);
    channel->listOfCarrierToNoiseL1.push_back(data.carrierToNoiseL1);
    channel->listOfCarrierToNoiseL2.push_back(data.carrierToNoiseL2);
}

void
PredictionsExperiments::experiment9WriteData(Experiment9SatelliteData *channel, Experiment9LearnindData& data) {
    if (!channel->listOfTec.empty()) {
        boost::posix_time::time_duration timeDiff = data.time.getUtcTime() - channel->lastLogTime.getUtcTime();

        if (timeDiff.total_milliseconds() != 20) {
            channel->listOfTec.clear();
            channel->listOfCarrierToNoiseL1.clear();
            channel->listOfCarrierToNoiseL2.clear();
            channel->sumOfTec = 0;
            channel->sumOfTec2 = 0;
            channel->sumOfCarrierToNoiseL1 = 0;
            channel->sumOfCarrierToNoiseL12 = 0;
            channel->sumOfCarrierToNoiseL2 = 0;
            channel->sumOfCarrierToNoiseL22 = 0;
        }

        if (channel->listOfTec.size() == numberOfElementsInExperiment9) {
            double sum_f = channel->sumOfTec / numberOfElementsInExperiment9;
            sum_f *= sum_f;
            double sum_f2 = channel->sumOfTec2 / numberOfElementsInExperiment9;
            double newSigmaOfTec = pow(sum_f2 - sum_f, 0.5);

            sum_f = channel->sumOfCarrierToNoiseL1 / numberOfElementsInExperiment9;
            sum_f *= sum_f;
            sum_f2 = channel->sumOfCarrierToNoiseL12 / numberOfElementsInExperiment9;
            double newSigmaOfCarrierToNoiseL1 = pow(sum_f2 - sum_f, 0.5);

            sum_f = channel->sumOfCarrierToNoiseL2 / numberOfElementsInExperiment9;
            sum_f *= sum_f;
            sum_f2 = channel->sumOfCarrierToNoiseL22 / numberOfElementsInExperiment9;
            double newSigmaOfCarrierToNoiseL2 = pow(sum_f2 - sum_f, 0.5);

            std::stringstream text;
            text << doubleToString(data.sigmaPhi, 10) << " | ";
            text << "satellite_system_id:" << data.satelliteSystemId << " ";
            text << "satellite_id:" << data.satelliteId << " ";
            text << "frequency:" << doubleToString(data.frequency, 0) << " ";
            text << "std_dev_tec:" << doubleToString(newSigmaOfTec, 10) << " ";
            text << "std_dev_carrier_to_noise_l1:" << doubleToString(newSigmaOfCarrierToNoiseL1, 10) << " ";
            text << "std_dev_carrier_to_noise_l2:" << doubleToString(newSigmaOfCarrierToNoiseL2, 10) << " ";
            text << "elevation_angle:" << doubleToString(data.elevationAngle, 1);

            printCout(text.rdbuf());

            channel->sumOfTec -= channel->listOfTec.front();
            channel->sumOfTec2 -= pow(channel->listOfTec.front(), 2.0);

            channel->sumOfCarrierToNoiseL1 -= channel->listOfCarrierToNoiseL1.front();
            channel->sumOfCarrierToNoiseL12 -= pow(channel->listOfCarrierToNoiseL1.front(), 2.0);

            channel->sumOfCarrierToNoiseL2 -= channel->listOfCarrierToNoiseL2.front();
            channel->sumOfCarrierToNoiseL22 -= pow(channel->listOfCarrierToNoiseL2.front(), 2.0);

            channel->listOfTec.pop_front();
            channel->listOfCarrierToNoiseL1.pop_front();
            channel->listOfCarrierToNoiseL2.pop_front();
        }

        channel->sumOfTec += data.tecL1L2;
        channel->sumOfTec2 += pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoiseL1 += data.carrierToNoiseL1;
        channel->sumOfCarrierToNoiseL12 += pow(data.carrierToNoiseL1, 2);

        channel->sumOfCarrierToNoiseL2 += data.carrierToNoiseL2;
        channel->sumOfCarrierToNoiseL22 += pow(data.carrierToNoiseL2, 2);
    } else {
        channel->sumOfTec = data.tecL1L2;
        channel->sumOfTec2 = pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoiseL1 = data.carrierToNoiseL1;
        channel->sumOfCarrierToNoiseL12 = pow(data.carrierToNoiseL1, 2);

        channel->sumOfCarrierToNoiseL2 = data.carrierToNoiseL2;
        channel->sumOfCarrierToNoiseL22 = pow(data.carrierToNoiseL2, 2);
    }

    channel->lastLogTime = data.time;

    channel->listOfTec.push_back(data.tecL1L2);
    channel->listOfCarrierToNoiseL1.push_back(data.carrierToNoiseL1);
    channel->listOfCarrierToNoiseL2.push_back(data.carrierToNoiseL2);
}

void
PredictionsExperiments::experiment10WriteData(Experiment10SatelliteData *channel, Experiment10LearnindData& data) {
    if (!channel->listOfTec.empty()) {
        boost::posix_time::time_duration timeDiff = data.time.getUtcTime() - channel->lastLogTime.getUtcTime();

        if (timeDiff.total_milliseconds() != 20) {
            channel->listOfTec.clear();
            channel->listOfCarrierToNoise.clear();
            channel->sumOfTec = 0;
            channel->sumOfTec2 = 0;
            channel->sumOfCarrierToNoise = 0;
            channel->sumOfCarrierToNoise2 = 0;
        }

        if (channel->listOfTec.size() == numberOfElementsInExperiment10) {
            double sum_f = channel->sumOfTec / numberOfElementsInExperiment10;
            sum_f *= sum_f;
            double sum_f2 = channel->sumOfTec2 / numberOfElementsInExperiment10;
            double newSigmaOfTec = pow(sum_f2 - sum_f, 0.5);

            sum_f = channel->sumOfCarrierToNoise / numberOfElementsInExperiment10;
            sum_f *= sum_f;
            sum_f2 = channel->sumOfCarrierToNoise2 / numberOfElementsInExperiment10;
            double newSigmaOfCarrierToNoise = pow(sum_f2 - sum_f, 0.5);

            std::stringstream text;
            text << doubleToString(data.sigmaPhi, 10) << " | ";
            text << "satellite_system_id:" << data.satelliteSystemId << " ";
            text << "satellite_id:" << data.satelliteId << " ";
            text << "frequency:" << doubleToString(data.frequency, 0) << " ";
            text << "std_dev_tec:" << doubleToString(newSigmaOfTec, 10) << " ";
            text << "std_dev_carrier_to_noise:" << doubleToString(newSigmaOfCarrierToNoise, 10) << " ";
            text << "elevation_angle:" << doubleToString(data.elevationAngle, 1);

            printCout(text.rdbuf());

            channel->sumOfTec -= channel->listOfTec.front();
            channel->sumOfTec2 -= pow(channel->listOfTec.front(), 2.0);

            channel->sumOfCarrierToNoise -= channel->listOfCarrierToNoise.front();
            channel->sumOfCarrierToNoise2 -= pow(channel->listOfCarrierToNoise.front(), 2.0);

            channel->listOfTec.pop_front();
            channel->listOfCarrierToNoise.pop_front();
        }

        channel->sumOfTec += data.tecL1L2;
        channel->sumOfTec2 += pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoise += data.carrierToNoise;
        channel->sumOfCarrierToNoise2 += pow(data.carrierToNoise, 2);
    } else {
        channel->sumOfTec = data.tecL1L2;
        channel->sumOfTec2 = pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoise = data.carrierToNoise;
        channel->sumOfCarrierToNoise2 = pow(data.carrierToNoise, 2);
    }

    channel->lastLogTime = data.time;

    channel->listOfTec.push_back(data.tecL1L2);
    channel->listOfCarrierToNoise.push_back(data.carrierToNoise);
}

void
PredictionsExperiments::experiment11WriteData(Experiment11SatelliteData *channel, Experiment11LearnindData& data) {
    if (!channel->listOfTec.empty()) {
        boost::posix_time::time_duration timeDiff = data.time.getUtcTime() - channel->lastLogTime.getUtcTime();

        if (timeDiff.total_milliseconds() != 20) {
            channel->listOfTec.clear();
            channel->listOfCarrierToNoise.clear();
            channel->sumOfTec = 0;
            channel->sumOfTec2 = 0;
            channel->sumOfCarrierToNoise = 0;
            channel->sumOfCarrierToNoise2 = 0;
        }

        if (channel->listOfTec.size() == numberOfElementsInExperiment11) {
            double sum_f = channel->sumOfTec / numberOfElementsInExperiment11;
            sum_f *= sum_f;
            double sum_f2 = channel->sumOfTec2 / numberOfElementsInExperiment11;
            double newSigmaOfTec = pow(sum_f2 - sum_f, 0.5);

            sum_f = channel->sumOfCarrierToNoise / numberOfElementsInExperiment11;
            sum_f *= sum_f;
            sum_f2 = channel->sumOfCarrierToNoise2 / numberOfElementsInExperiment11;
            double newSigmaOfCarrierToNoise = pow(sum_f2 - sum_f, 0.5);

            std::stringstream text;
            text << doubleToString(data.s4, 10) << " | ";
            text << "satellite_system_id:" << data.satelliteSystemId << " ";
            text << "satellite_id:" << data.satelliteId << " ";
            text << "frequency:" << doubleToString(data.frequency, 0) << " ";
            text << "std_dev_tec:" << doubleToString(newSigmaOfTec, 10) << " ";
            text << "std_dev_carrier_to_noise:" << doubleToString(newSigmaOfCarrierToNoise, 10) << " ";
            text << "elevation_angle:" << doubleToString(data.elevationAngle, 1);

            printCout(text.rdbuf());

            channel->sumOfTec -= channel->listOfTec.front();
            channel->sumOfTec2 -= pow(channel->listOfTec.front(), 2.0);

            channel->sumOfCarrierToNoise -= channel->listOfCarrierToNoise.front();
            channel->sumOfCarrierToNoise2 -= pow(channel->listOfCarrierToNoise.front(), 2.0);

            channel->listOfTec.pop_front();
            channel->listOfCarrierToNoise.pop_front();
        }

        channel->sumOfTec += data.tecL1L2;
        channel->sumOfTec2 += pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoise += data.carrierToNoise;
        channel->sumOfCarrierToNoise2 += pow(data.carrierToNoise, 2);
    } else {
        channel->sumOfTec = data.tecL1L2;
        channel->sumOfTec2 = pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoise = data.carrierToNoise;
        channel->sumOfCarrierToNoise2 = pow(data.carrierToNoise, 2);
    }

    channel->lastLogTime = data.time;
    channel->listOfTec.push_back(data.tecL1L2);
    channel->listOfCarrierToNoise.push_back(data.carrierToNoise);
}

void
PredictionsExperiments::experiment12WriteData(Experiment12SatelliteData *channel, Experiment12LearnindData& data) {
    if (!channel->listOfTec.empty()) {
        boost::posix_time::time_duration timeDiff = data.time.getUtcTime() - channel->lastLogTime.getUtcTime();

        if (timeDiff.total_milliseconds() != 20) {
            channel->listOfTec.clear();
            channel->sumOfTec = 0;
            channel->sumOfTec2 = 0;
        }

        if (channel->listOfTec.size() == numberOfElementsInExperiment12) {
            double sum_f = channel->sumOfTec / numberOfElementsInExperiment12;
            sum_f *= sum_f;
            double sum_f2 = channel->sumOfTec2 / numberOfElementsInExperiment12;
            double newSigmaOfTec = pow(sum_f2 - sum_f, 0.5);

            std::stringstream text;
            text << doubleToString(data.sigmaPhi, 10) << " | ";
            text << "satellite_system_id:" << data.satelliteSystemId << " ";
            text << "satellite_id:" << data.satelliteId << " ";
            text << "frequency:" << doubleToString(data.frequency, 0) << " ";
            text << "std_dev_tec:" << doubleToString(newSigmaOfTec, 10) << " ";
            text << "elevation_angle:" << doubleToString(data.elevationAngle, 1);

            printCout(text.rdbuf());

            channel->sumOfTec -= channel->listOfTec.front();
            channel->sumOfTec2 -= pow(channel->listOfTec.front(), 2.0);

            channel->listOfTec.pop_front();
        }

        channel->sumOfTec += data.tecL1L2;
        channel->sumOfTec2 += pow(data.tecL1L2, 2);
    } else {
        channel->sumOfTec = data.tecL1L2;
        channel->sumOfTec2 = pow(data.tecL1L2, 2);
    }

    channel->lastLogTime = data.time;
    channel->listOfTec.push_back(data.tecL1L2);
}

void
PredictionsExperiments::experiment13WriteData(Experiment13SatelliteData *channel, Experiment13LearnindData& data) {
    if (!channel->listOfTec.empty()) {
        boost::posix_time::time_duration timeDiff = data.time.getUtcTime() - channel->lastLogTime.getUtcTime();

        if (timeDiff.total_milliseconds() != 20) {
            channel->listOfTec.clear();
            channel->listOfCarrierToNoise.clear();
            channel->sumOfTec = 0;
            channel->sumOfTec2 = 0;
            channel->sumOfCarrierToNoise = 0;
            channel->sumOfCarrierToNoise2 = 0;
        }

        if (channel->listOfTec.size() == numberOfElementsInExperiment11) {
            double sum_f = channel->sumOfTec / numberOfElementsInExperiment11;
            sum_f *= sum_f;
            double sum_f2 = channel->sumOfTec2 / numberOfElementsInExperiment11;
            double newSigmaOfTec = pow(sum_f2 - sum_f, 0.5);

            sum_f = channel->sumOfCarrierToNoise / numberOfElementsInExperiment11;
            sum_f *= sum_f;
            sum_f2 = channel->sumOfCarrierToNoise2 / numberOfElementsInExperiment11;
            double newSigmaOfCarrierToNoise = pow(sum_f2 - sum_f, 0.5);

            std::stringstream text;
            text << doubleToString(data.s4, 10) << " | ";
            text << "satellite_system_id:" << data.satelliteSystemId << " ";
            text << "frequency:" << doubleToString(data.frequency, 0) << " ";
            text << "std_dev_tec:" << doubleToString(newSigmaOfTec, 10) << " ";
            text << "std_dev_carrier_to_noise:" << doubleToString(newSigmaOfCarrierToNoise, 10) << " ";
            text << "elevation_angle:" << doubleToString(data.elevationAngle, 1);

            printCout(text.rdbuf());

            channel->sumOfTec -= channel->listOfTec.front();
            channel->sumOfTec2 -= pow(channel->listOfTec.front(), 2.0);

            channel->sumOfCarrierToNoise -= channel->listOfCarrierToNoise.front();
            channel->sumOfCarrierToNoise2 -= pow(channel->listOfCarrierToNoise.front(), 2.0);

            channel->listOfTec.pop_front();
            channel->listOfCarrierToNoise.pop_front();
        }

        channel->sumOfTec += data.tecL1L2;
        channel->sumOfTec2 += pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoise += data.carrierToNoise;
        channel->sumOfCarrierToNoise2 += pow(data.carrierToNoise, 2);
    } else {
        channel->sumOfTec = data.tecL1L2;
        channel->sumOfTec2 = pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoise = data.carrierToNoise;
        channel->sumOfCarrierToNoise2 = pow(data.carrierToNoise, 2);
    }

    channel->lastLogTime = data.time;
    channel->listOfTec.push_back(data.tecL1L2);
    channel->listOfCarrierToNoise.push_back(data.carrierToNoise);
}

void
PredictionsExperiments::experiment14WriteData(Experiment14SatelliteData *channel, Experiment14LearnindData& data) {
    if (!channel->listOfTec.empty()) {
        boost::posix_time::time_duration timeDiff = data.time.getUtcTime() - channel->lastLogTime.getUtcTime();

        if (timeDiff.total_milliseconds() != 20) {
            channel->listOfTec.clear();
            channel->listOfCarrierToNoiseL1.clear();
            channel->listOfCarrierToNoiseL2.clear();
            channel->sumOfTec = 0;
            channel->sumOfTec2 = 0;
            channel->sumOfCarrierToNoiseL1 = 0;
            channel->sumOfCarrierToNoiseL12 = 0;
            channel->sumOfCarrierToNoiseL2 = 0;
            channel->sumOfCarrierToNoiseL22 = 0;
        }

        if (channel->listOfTec.size() == numberOfElementsInExperiment3) {
            double sum_f = channel->sumOfTec / numberOfElementsInExperiment3;
            sum_f *= sum_f;
            double sum_f2 = channel->sumOfTec2 / numberOfElementsInExperiment3;
            double newSigmaOfTec = pow(sum_f2 - sum_f, 0.5);

            sum_f = channel->sumOfCarrierToNoiseL1 / numberOfElementsInExperiment3;
            sum_f *= sum_f;
            sum_f2 = channel->sumOfCarrierToNoiseL12 / numberOfElementsInExperiment3;
            double newSigmaOfCarrierToNoiseL1 = pow(sum_f2 - sum_f, 0.5);

            sum_f = channel->sumOfCarrierToNoiseL2 / numberOfElementsInExperiment3;
            sum_f *= sum_f;
            sum_f2 = channel->sumOfCarrierToNoiseL22 / numberOfElementsInExperiment3;
            double newSigmaOfCarrierToNoiseL2 = pow(sum_f2 - sum_f, 0.5);

            std::stringstream text;
            text << doubleToString(data.s4, 10) << " | ";
            text << "satellite_system_id:" << data.satelliteSystemId << " ";
//            text << "satellite_id:" << data.satelliteId << " "; //like 8, but without this useless subject
            text << "frequency:" << doubleToString(data.frequency, 0) << " ";
            text << "std_dev_tec:" << doubleToString(newSigmaOfTec, 10) << " ";
            text << "std_dev_carrier_to_noise_l1:" << doubleToString(newSigmaOfCarrierToNoiseL1, 10) << " ";
            text << "std_dev_carrier_to_noise_l2:" << doubleToString(newSigmaOfCarrierToNoiseL2, 10) << " ";
            text << "elevation_angle:" << doubleToString(data.elevationAngle, 1);

            printCout(text.rdbuf());

            channel->sumOfTec -= channel->listOfTec.front();
            channel->sumOfTec2 -= pow(channel->listOfTec.front(), 2.0);

            channel->sumOfCarrierToNoiseL1 -= channel->listOfCarrierToNoiseL1.front();
            channel->sumOfCarrierToNoiseL12 -= pow(channel->listOfCarrierToNoiseL1.front(), 2.0);

            channel->sumOfCarrierToNoiseL2 -= channel->listOfCarrierToNoiseL2.front();
            channel->sumOfCarrierToNoiseL22 -= pow(channel->listOfCarrierToNoiseL2.front(), 2.0);

            channel->listOfTec.pop_front();
            channel->listOfCarrierToNoiseL1.pop_front();
            channel->listOfCarrierToNoiseL2.pop_front();
        }

        channel->sumOfTec += data.tecL1L2;
        channel->sumOfTec2 += pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoiseL1 += data.carrierToNoiseL1;
        channel->sumOfCarrierToNoiseL12 += pow(data.carrierToNoiseL1, 2);

        channel->sumOfCarrierToNoiseL2 += data.carrierToNoiseL2;
        channel->sumOfCarrierToNoiseL22 += pow(data.carrierToNoiseL2, 2);
    } else {
        channel->sumOfTec = data.tecL1L2;
        channel->sumOfTec2 = pow(data.tecL1L2, 2);

        channel->sumOfCarrierToNoiseL1 = data.carrierToNoiseL1;
        channel->sumOfCarrierToNoiseL12 = pow(data.carrierToNoiseL1, 2);

        channel->sumOfCarrierToNoiseL2 = data.carrierToNoiseL2;
        channel->sumOfCarrierToNoiseL22 = pow(data.carrierToNoiseL2, 2);
    }

    channel->lastLogTime = data.time;

    channel->listOfTec.push_back(data.tecL1L2);
    channel->listOfCarrierToNoiseL1.push_back(data.carrierToNoiseL1);
    channel->listOfCarrierToNoiseL2.push_back(data.carrierToNoiseL2);
}

void
PredictionsExperiments::experiment15WriteData(Experiment15SatelliteData *channel, Experiment15LearnindData& data) {
    std::stringstream text;
    text << data.time.getStringUtcTime() << ";";
    text << doubleToString(data.s4, 10)  << ";";
    text << "satellite_system_id:" << data.satelliteSystemId << ";";
    text << "satellite_id:" << data.satelliteId << ";";
    text << "frequency:" << doubleToString(data.frequency, 0) << ";";
    text << "tec:" << doubleToString(data.tecL1L2, 10) << ";";
    text << "carrier_to_noise_l1:" << doubleToString(data.carrierToNoiseL1, 10) << ";";
    text << "carrier_to_noise_l2:" << doubleToString(data.carrierToNoiseL2, 10) << ";";
    text << "elevation_angle:" << doubleToString(data.elevationAngle, 1);
    printCout(text.str());
}

void
PredictionsExperiments::experiment18GPSL1WriteData(Experiment18LearnindData& data) {
    size_t i = (size_t) (data.s4 / experiment18GPSL1_.COUNTERS_STEP);
    experiment18GPSL1_.counters.at(i)++;
}

void
PredictionsExperiments::experiment18GLOL1WriteData(Experiment18LearnindData& data) {
    size_t i = (size_t) (data.s4 / experiment18GLOL1_.COUNTERS_STEP);
    experiment18GLOL1_.counters.at(i)++;
}

void
PredictionsExperiments::experiment18GLOL2WriteData(Experiment18LearnindData& data) {
    size_t i = (size_t) (data.s4 / experiment18GLOL2_.COUNTERS_STEP);
    experiment18GLOL2_.counters.at(i)++;
}