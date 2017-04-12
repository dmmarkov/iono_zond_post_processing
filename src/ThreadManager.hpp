/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   ThreadManager.hpp
 * Author: dmitri
 *
 * Created on 26 февраля 2017 г., 8:24
 */

#ifndef THREADMANAGER_HPP
#define THREADMANAGER_HPP

#include <atomic>
#include <cstdlib>
#include <mutex>
#include <map>
#include <thread>

#include <boost/thread/mutex.hpp>

#include "ICalculation.hpp"

class ThreadManager {
public:
    static ThreadManager&
    getInstance() {
        static ThreadManager currentInstance;
        return currentInstance;
    }

    void start(size_t numberOfThreads);
    void stop();

    void addNewCalculation(ICalculation *calculation);
    void releaseCalculation(ICalculation *calculation);
    void releaseAll();

    void someOneUsedCout();
    bool isReadyAcceptNewData();
private:
    struct threadCalculation {
        std::vector<ICalculation*> order;
        boost::timed_mutex lockOrder;
    };

    std::atomic<bool> isStopThreads_;
    std::atomic<bool> isPaused_;
    std::atomic<bool> isSomeOneUsedCout_;
    std::atomic<bool> isReadyAcceptNewData_;

    std::vector<ICalculation*> calculations_;

    size_t sizeOfCalculations_;
    size_t numberOfThreads_;

    boost::thread_group threads_;

    boost::timed_mutex lockCalcutaions_;

    std::map<size_t, threadCalculation*> processMap_;

    void useMergeData(size_t threadNumber);
    void useCalculateData(size_t threadNumber);
    void makeProcessThreadOrder();
    void updateAcceptStatus();
    void updateAcceptStatusHandler(const boost::system::error_code& error);

    ThreadManager();
    ThreadManager(const ThreadManager& orig)  = delete;
    virtual ~ThreadManager();
};

#endif /* THREADMANAGER_HPP */

