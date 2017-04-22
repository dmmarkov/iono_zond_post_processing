/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   PredictionsExperiments.hpp
 * Author: dmitri
 *
 * Created on 26 февраля 2017 г., 22:04
 */

#ifndef PREDICTIONSEXPERIMENTS_HPP
#define PREDICTIONSEXPERIMENTS_HPP

#include <atomic>
#include <iostream>

#include "ICalculation.hpp"
#include "ISatellite.hpp"
#include "IonoZondExperiment16.hpp"
#include "IonoZondExperiment17.hpp"

struct Experiment1LearnindData {
    double s4;

    int satelliteSystemId;
    int satelliteId;
    int glonassFrequency;
    double elevationAngle;
    double codePseudorande;
    double phasePseudorande;
    double carrierToNoise;
    double frequency;
};

struct Experiment2LearnindData {
    double codePseudorande;

    GnssLogTime time;

    int satelliteSystemId;
    int satelliteId;
    double frequency;
    double elevationAngle;
    double carrierToNoise;
};

struct Experiment3LearnindData {
    double s4;

    double frequency;
    int satelliteId;
    int satelliteSystemId;

    double tecL1L2;
    double carrierToNoiseL1;
    double carrierToNoiseL2;

    GnssLogTime time;
};

struct Experiment4LearnindData {
    double sigmaPhi;

    double frequency;
    int satelliteId;
    int satelliteSystemId;

    double tecL1L2;
    double carrierToNoiseL1;
    double carrierToNoiseL2;

    GnssLogTime time;
};

struct Experiment5LearnindData {
    double s4;

    double frequency;
    int satelliteId;
    int satelliteSystemId;

    double sigmaPhi;
    double elevationAngle;

    GnssLogTime time;
};

struct Experiment6LearnindData {
    double s4;

    double frequency;
    int satelliteId;
    int satelliteSystemId;

    double tecL1L2;
    double carrierToNoiseL1;
    double carrierToNoiseL2;
    double elevationAngle;

    GnssLogTime time;
};

struct Experiment7LearnindData {
    double sigmaPhi;

    double frequency;
    int satelliteId;
    int satelliteSystemId;

    double tecL1L2;
    double carrierToNoiseL1;
    double carrierToNoiseL2;
    double elevationAngle;

    GnssLogTime time;
};

struct Experiment8LearnindData {
    double s4;

    double frequency;
    int satelliteId;
    int satelliteSystemId;

    double tecL1L2;
    double carrierToNoiseL1;
    double carrierToNoiseL2;
    double elevationAngle;

    GnssLogTime time;
};

struct Experiment9LearnindData {
    double sigmaPhi;

    double frequency;
    int satelliteId;
    int satelliteSystemId;

    double tecL1L2;
    double carrierToNoiseL1;
    double carrierToNoiseL2;
    double elevationAngle;

    GnssLogTime time;
};

struct Experiment10LearnindData {
    double sigmaPhi;

    double frequency;
    int satelliteId;
    int satelliteSystemId;

    double tecL1L2;
    double carrierToNoise;
    double elevationAngle;

    GnssLogTime time;
};

struct Experiment11LearnindData {
    double s4;

    double frequency;
    int satelliteId;
    int satelliteSystemId;

    double tecL1L2;
    double carrierToNoise;
    double elevationAngle;

    GnssLogTime time;
};

struct Experiment12LearnindData {
    double sigmaPhi;

    double frequency;
    int satelliteId;
    int satelliteSystemId;

    double tecL1L2;
    double elevationAngle;

    GnssLogTime time;
};

struct Experiment16LearnindData {
    double s4;

    double frequency;
    int satelliteId;
    int satelliteSystemId;

    GnssLogTime time;
};

struct Experiment18LearnindData {
    double s4;

    int satelliteSystemId;
    int satelliteId;
    double frequency;
};

typedef Experiment11LearnindData Experiment13LearnindData;
typedef Experiment8LearnindData Experiment14LearnindData;
typedef Experiment8LearnindData Experiment15LearnindData;

struct Experiment2ChannelData {
    double codePseudorande;
    double elevationAngle;
    double carrierToNoise;
};

struct Experiment2SatelliteChannelData {
    GnssLogTime lastLogTime;
    int satelliteSystem;
    int satelliteId;
    double frequency;

    std::list<Experiment2ChannelData> listOfData;
};

struct Experiment3SatelliteData {
    GnssLogTime lastLogTime;
    int satelliteSystem;
    int satelliteId;
    double frequency;

    std::list<double> listOfTec;
    std::list<double> listOfCarrierToNoiseL1;
    std::list<double> listOfCarrierToNoiseL2;

    long double sumOfTec;
    long double sumOfTec2;
    long double sumOfCarrierToNoiseL1;
    long double sumOfCarrierToNoiseL12;
    long double sumOfCarrierToNoiseL2;
    long double sumOfCarrierToNoiseL22;
};

typedef Experiment3SatelliteData Experiment4SatelliteData;

struct Experiment6SatelliteData {
    GnssLogTime lastLogTime;
    int satelliteSystem;
    int satelliteId;
    double frequency;

    std::list<double> listOfTec;

    long double sumOfTec;
    long double sumOfTec2;
};

typedef Experiment6SatelliteData Experiment7SatelliteData;
typedef Experiment3SatelliteData Experiment8SatelliteData;
typedef Experiment3SatelliteData Experiment9SatelliteData;

struct Experiment10SatelliteData {
    GnssLogTime lastLogTime;
    int satelliteSystem;
    int satelliteId;
    double frequency;

    std::list<double> listOfTec;
    std::list<double> listOfCarrierToNoise;

    long double sumOfTec;
    long double sumOfTec2;
    long double sumOfCarrierToNoise;
    long double sumOfCarrierToNoise2;
};

struct Experiment16SatelliteData {
    GnssLogTime lastLogTime;
    int satelliteSystem;
    int satelliteId;
    double frequency;

    int sizeOfListOfS4;
    std::list<std::string> listOfS4;
};

struct Experiment18SatelliteData {
    double MIN_S4_VALUE;
    double MAX_S4_VALUE;
    size_t INTERVAL_COUNT;
    double COUNTERS_STEP;

    std::vector<size_t> counters;

    Experiment18SatelliteData() {
        MIN_S4_VALUE = 0.0;
        MAX_S4_VALUE = 1.0;
        INTERVAL_COUNT = 64;
        COUNTERS_STEP = (MAX_S4_VALUE - MIN_S4_VALUE) / (double) INTERVAL_COUNT;

        counters.resize(INTERVAL_COUNT);
    }
};

typedef Experiment10SatelliteData Experiment11SatelliteData;
typedef Experiment6SatelliteData Experiment12SatelliteData;
typedef Experiment11SatelliteData Experiment13SatelliteData;
typedef Experiment8SatelliteData Experiment14SatelliteData;
typedef Experiment8SatelliteData Experiment15SatelliteData;

class PredictionsExperiments : public ICalculation {
public:
    explicit PredictionsExperiments(ISatellite* satellite,
                                    unsigned short week,
                                    unsigned int milliseconds,
                                    int experimentId,
                                    int filterSatelliteSystemId,
                                    int filterSatelliteId,
                                    int filterChannelId);
    virtual ~PredictionsExperiments();

    virtual void mergeData() override;
    virtual void calculateData() override;

    virtual void stopThreads() override;

    virtual void closePlotData() override;
    virtual void forceCloseOutDataFiles(GnssLogTime& time) override;

    ISatellite* satellite_;

    void pushS4ValuesL1(double s4, unsigned short week, unsigned int milliseconds);
    void pushS4ValuesL2(double s4, unsigned short week, unsigned int milliseconds);

    void pushS4WithoutNoiseValuesL1(double s4, unsigned short week, unsigned int milliseconds);
    void pushS4WithoutNoiseValuesL2(double s4, unsigned short week, unsigned int milliseconds);

    void pushSigmaPhiValuesL1(double sigmaPhi, unsigned short week, unsigned int milliseconds);
    void pushSigmaPhiValuesL2(double sigmaPhi, unsigned short week, unsigned int milliseconds);
    void pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds);
    void pushValuesForTec(double PsrML1, double PsrML2,
                          double PsrFL1, double PsrFL2,
                          double carrierToNoiseL1, double carrierToNoiseL2,
                          double frequencyL1, double frequencyL2,
                          double waveLengthL1, double waveLengthL2,
                          double tec,
                          unsigned short week, unsigned int milliseconds);
private:
    std::atomic<bool> isStopThreads_;

    std::string tmpFileName_;
    std::string outFileName_;

    std::fstream outFile_;

    int experimentId_;
    int filterSatelliteSystemId_;
    int filterSatelliteId_;
    int filterSatelliteChannelId_;

    int satelliteSystemId_;
    int glonassFrequency_;

    struct S4Value {
        double s4;
        unsigned short week;
        unsigned int milliseconds;
    };

    struct SigmaPhiValue {
        double sigmaPhi;
        unsigned short week;
        unsigned int milliseconds;
    };

    struct ElevationAngleValue {
        double elevationAngle;
        unsigned short week;
        unsigned int milliseconds;
    };

    struct ValuesForTec {
        double PsrML1;
        double PsrML2;
        double PsrFL1;
        double PsrFL2;
        double carrierToNoiseL1;
        double carrierToNoiseL2;
        double frequencyL1;
        double frequencyL2;
        double waveLengthL1;
        double waveLengthL2;
        unsigned short week;
        unsigned int milliseconds;
        double tec;
    };

    boost::mutex mergeLock_;

    GnssDataMeasurements2<S4Value> s4ValuesL1;
    GnssDataMeasurements2<S4Value> s4ValuesL2;
    GnssDataMeasurements2<S4Value> s4WithoutNoiseValuesL1;
    GnssDataMeasurements2<S4Value> s4WithoutNoiseValuesL2;
    GnssDataMeasurements2<SigmaPhiValue> sigmaPhiValuesL1;
    GnssDataMeasurements2<SigmaPhiValue> sigmaPhiValuesL2;
    GnssDataMeasurements2<ElevationAngleValue> ElevationAngleValues;
    GnssDataMeasurements2<ValuesForTec> ValuesForTecValues;

    Experiment2SatelliteChannelData experiment2L1;
    Experiment2SatelliteChannelData experiment2L2;
    Experiment3SatelliteData        experiment3L1;
    Experiment4SatelliteData        experiment4L1;
    Experiment4SatelliteData        experiment4L2;
    Experiment6SatelliteData        experiment6;
    Experiment7SatelliteData        experiment7L1;
    Experiment7SatelliteData        experiment7L2;
    Experiment8SatelliteData        experiment8L1;
    Experiment8SatelliteData        experiment8L2;
    Experiment9SatelliteData        experiment9L1;
    Experiment9SatelliteData        experiment9L2;
    Experiment10SatelliteData       experiment10L1;
    Experiment10SatelliteData       experiment10L2;
    Experiment11SatelliteData       experiment11L1;
    Experiment11SatelliteData       experiment11L2;
    Experiment12SatelliteData       experiment12L1;
    Experiment12SatelliteData       experiment12L2;
    Experiment13SatelliteData       experiment13L1;
    Experiment13SatelliteData       experiment13L2;
    Experiment14SatelliteData       experiment14L1;
    Experiment14SatelliteData       experiment14L2;
    Experiment15SatelliteData       experiment15L1;
    Experiment15SatelliteData       experiment15L2;
    IonoZondExperiment16           *experiment16L1_;
    IonoZondExperiment16           *experiment16L2_;
    IonoZondExperiment17           *experiment17L1_;
    IonoZondExperiment17           *experiment17L2_;
    Experiment18SatelliteData       experiment18GPSL1_;
    Experiment18SatelliteData       experiment18GLOL1_;
    Experiment18SatelliteData       experiment18GLOL2_;

    void experiment1WriteData(Experiment1LearnindData& data);
    void experiment2WriteData(Experiment2SatelliteChannelData *channel, Experiment2LearnindData& data);
    void experiment3WriteData(Experiment3SatelliteData *channel, Experiment3LearnindData& data);
    void experiment4WriteData(Experiment4SatelliteData *channel, Experiment4LearnindData& data);
    void experiment5WriteData(Experiment5LearnindData& data);
    void experiment6WriteData(Experiment6SatelliteData *channel, Experiment6LearnindData& data);
    void experiment7WriteData(Experiment7SatelliteData *channel, Experiment7LearnindData& data);
    void experiment8WriteData(Experiment8SatelliteData *channel, Experiment8LearnindData& data);
    void experiment9WriteData(Experiment9SatelliteData *channel, Experiment9LearnindData& data);
    void experiment10WriteData(Experiment10SatelliteData *channel, Experiment10LearnindData& data);
    void experiment11WriteData(Experiment11SatelliteData *channel, Experiment11LearnindData& data);
    void experiment12WriteData(Experiment12SatelliteData *channel, Experiment12LearnindData& data);
    void experiment13WriteData(Experiment13SatelliteData *channel, Experiment13LearnindData& data);
    void experiment14WriteData(Experiment14SatelliteData *channel, Experiment14LearnindData& data);
    void experiment15WriteData(Experiment15SatelliteData *channel, Experiment15LearnindData& data);
    void experiment18GPSL1WriteData(Experiment18LearnindData& data);
    void experiment18GLOL1WriteData(Experiment18LearnindData& data);
    void experiment18GLOL2WriteData(Experiment18LearnindData& data);

    PredictionsExperiments(const PredictionsExperiments& orig) = delete;
};

#endif /* PREDICTIONSEXPERIMENTS_HPP */

