/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   S4IrregulationsSearch.hpp
 * Author: dmitri
 *
 * Created on 6 Июнь 2016 г., 22:34
 */

#ifndef S4IRREGULATIONSSEARCH_HPP
#define S4IRREGULATIONSSEARCH_HPP

#include <list>
#include <fstream>
#include <atomic>

#include <boost/thread.hpp>
#include <mutex>

#include "ISatelliteChannel.hpp"

class S4IrregulationsSearch {
public:
    explicit S4IrregulationsSearch(ISatelliteChannel* satelliteChannel);
    virtual ~S4IrregulationsSearch();

    void pushS4Value(unsigned short week, unsigned int milliseconds, double s4value);
    void stopThreads();
private:
    ISatelliteChannel* satelliteChannel_;

    struct S4Data {
        unsigned short week;
        unsigned int milliseconds;
        double s4value;
    };

    std::list<S4Data*> listWriteOfS4Data_;
    std::list<S4Data*> listReadOfS4Data_;

    boost::mutex mutexLockPush_;

    std::atomic<bool> isStopThreads_;

    std::fstream outFile_;

    void calculateData();
    void moveDataToReadList();

    S4IrregulationsSearch(const S4IrregulationsSearch& orig) = delete;
    S4IrregulationsSearch& operator=(const S4IrregulationsSearch& ) = delete;
};

#endif /* S4IRREGULATIONSSEARCH_HPP */

