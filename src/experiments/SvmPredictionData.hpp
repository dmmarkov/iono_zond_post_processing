/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SvmPredictionData.hpp
 * Author: dmitri
 *
 * Created on 13 Июнь 2016 г., 17:24
 */

#ifndef SVMPREDICTIONDATA_HPP
#define SVMPREDICTIONDATA_HPP

#include <string>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "ISatellite.hpp"
#include "ISatelliteChannel.hpp"
#include "PredictionsExperiments.hpp"

class SvmPredictionData {
public:

    static SvmPredictionData&
    getInstance() {
        static SvmPredictionData currentInstance;
        return currentInstance;
    }

    void start(std::string& nameOfExperiment, std::string& nameOfSatelliteSystem, std::string& idOfSatellite, std::string& nameOfSatelliteChannel);
    void stop();
    void restart();

    void addSatellite(ISatellite* satellite,
            unsigned short week,
            unsigned int milliseconds);
    void closeSatellite(ISatellite* satellite);

    void pushS4Value(ISatellite* satellite,
            ISatelliteChannel* channel,
            double s4,
            unsigned short week,
            unsigned int milliseconds);

    void pushS4WithoutNoiseValue(ISatellite* satellite,
            ISatelliteChannel* channel,
            double s4,
            unsigned short week,
            unsigned int milliseconds);

    void pushSigmaPhiValue(ISatellite* satellite,
            ISatelliteChannel* channel,
            double sigmaPhi,
            unsigned short week,
            unsigned int milliseconds);

    void pushElevationAngle(ISatellite* satellite,
            double elevationAngle,
            unsigned short week,
            unsigned int milliseconds);

    void pushValuesForTec(ISatellite* satellite,
            double PsrML1,
            double PsrML2,
            double PsrFL1,
            double PsrFL2,
            double carrierToNoiseL1,
            double carrierToNoiseL2,
            double frequencyL1,
            double frequencyL2,
            double waveLengthL1,
            double waveLengthL2,
            unsigned short week,
            unsigned int milliseconds,
            double tec);
private:
    std::map<int, PredictionsExperiments*> satellites_;
    std::atomic<bool> isActive_;

    boost::shared_mutex mutexLockSatelliteAndChannels_;

    int numberOfExperiment_;

    //it is only for my comments about experiments
    bool isExperiment0; //only output of merge data
    bool isExperiment1; //output S4 L1 raw data to WV
    bool isExperiment2; //output pseudorange, try to predict +1 value
    bool isExperiment3; //s4 by std dev of tec, c/n0 l1 l2
    bool isExperiment4; //SigmaPhi by std dev of tec, c/n0 l1 l2
    bool isExperiment5; //s4 by SigmaPhi
    bool isExperiment6; //s4 by std dev of tec, current value of c/n0 l1 and l2, elevation angle
    bool isExperiment7; //SigmaPhi by std dev of tec, current value of c/n0 l1 l2, elevation angle
    bool isExperiment8; //s4 by std dev of tec, std dev of c/n0 l1, std dev of l2, elevation angle
    bool isExperiment9; //SigmaPhi by std dev of tec, std dev of c/n0 l1, std dev of l2, elevation angle
    bool isExperiment10; //SigmaPhi by std dev of tec, std dev of c/n0 this channel, elevation angle
    bool isExperiment11; //s4 by std dev of tec, std dev of c/n0 this channel, elevation angle
    bool isExperiment12; //SigmaPhi by std dev of tec, elevation angle
    bool isExperiment13; //experiment 11 without satellite_id
    bool isExperiment14; //experiment 8 without satellite_id
    bool isExperiment15; //only date+time and s4 values for autocorreleation
    bool isExperiment16; //predicting S4 from previous 2999 values.
    bool isExperiment17; //like 16' but with lower precision. Only 4 values after 0
    bool isExperiment18; //it's frequency of occurence of S4 in intervals

    int filterSatelliteSystem;
    int filterSatelliteId;
    int filterSatelliteChannel;

    int getKeyId(ISatellite* satellite);

    SvmPredictionData();
    virtual ~SvmPredictionData();
    SvmPredictionData(const SvmPredictionData& orig) = delete;
};

#endif /* SVMPREDICTIONDATA_HPP */

