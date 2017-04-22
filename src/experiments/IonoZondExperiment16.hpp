/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   IonoZondExperiment16.hpp
 * Author: dmitri
 *
 * Created on 18 февраля 2017 г., 17:26
 */

#ifndef IONOZONDEXPERIMENT16_HPP
#define IONOZONDEXPERIMENT16_HPP

#include <boost/thread/thread.hpp>

#include "GnssDataMeasurements2.hpp"
#include "GnssLogTime.hpp"
#include "common_functions.hpp"
#include "ICalculation.hpp"

class Experiment16Worker : ICalculation {
    public:
        struct Data {
            unsigned short week_;
            unsigned int milliseconds_;
            double s4;
        };

        explicit Experiment16Worker(const int satelliteSystem, const int satelliteId, const double frequency);
        virtual ~Experiment16Worker();

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

        Experiment16Worker(const Experiment16Worker& orig) = delete;
        Experiment16Worker& operator=(const Experiment16Worker& ) = delete;
};


class IonoZondExperiment16 : ICalculation {
public:
    explicit IonoZondExperiment16(const int satelliteSystem, const int satelliteId, const double frequency);
    virtual ~IonoZondExperiment16();

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
    std::vector<Experiment16Worker*> workers;
    long int lastMilliseconsTime_;

    IonoZondExperiment16(const IonoZondExperiment16& orig) = delete;
    IonoZondExperiment16& operator=(const IonoZondExperiment16& ) = delete;
};

#endif /* IONOZONDEXPERIMENT16_HPP */

