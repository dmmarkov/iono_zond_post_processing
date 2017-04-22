/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   IonoZondExperiment17.hpp
 * Author: dmitri
 *
 * Created on 18 февраля 2017 г., 17:26
 */

#ifndef IONOZONDEXPERIMENT17_HPP
#define IONOZONDEXPERIMENT17_HPP

#include <boost/thread/thread.hpp>

#include "GnssDataMeasurements2.hpp"
#include "GnssLogTime.hpp"
#include "common_functions.hpp"
#include "ICalculation.hpp"

class Experiment17Worker : ICalculation {
    public:
        struct Data {
            unsigned short week_;
            unsigned int milliseconds_;
            double s4;
        };

        explicit Experiment17Worker(const int satelliteSystem, const int satelliteId, const double frequency);
        virtual ~Experiment17Worker();

        void pushExperimentData(const unsigned short week, unsigned int milliseconds, const double s4);

        virtual void stopThreads() override;

        virtual void mergeData() override;
        virtual void calculateData() override;

        virtual void closePlotData() override;
        virtual void forceCloseOutDataFiles(GnssLogTime& time) override;
    private:
        std::atomic<bool> isStopThreads_;

        GnssDataMeasurements2<Data> s4Data_;

        size_t sizeOfStringBuffer_;
        char *stringBuffer_;

        int satelliteSystem_;
        int satelliteId_;
        double frequency_;

        boost::mutex calcLock_;

        Experiment17Worker(const Experiment17Worker& orig) = delete;
        Experiment17Worker& operator=(const Experiment17Worker& ) = delete;
};


class IonoZondExperiment17 : ICalculation {
public:
    explicit IonoZondExperiment17(const int satelliteSystem, const int satelliteId, const double frequency);
    virtual ~IonoZondExperiment17();

    void pushExperimentData(GnssLogTime *logTime, const double s4);

    virtual void stopThreads() override;

    virtual void mergeData() override;
    virtual void calculateData() override;

    virtual void closePlotData() override;
    virtual void forceCloseOutDataFiles(GnssLogTime& time) override;
private:
    int satelliteSystem_;
    int satelliteId_;
    double frequency_;

    std::atomic<bool> isStopThreads_;
    std::atomic<size_t> countOfPushed_;
    boost::mutex mutexLock_;

    std::vector<Experiment17Worker*> workers;

    long int lastMilliseconsTime_;

    IonoZondExperiment17(const IonoZondExperiment17& orig) = delete;
    IonoZondExperiment17& operator=(const IonoZondExperiment17& ) = delete;
};

#endif /* IONOZONDEXPERIMENT17_HPP */

