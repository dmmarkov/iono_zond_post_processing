/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   ThreadManager.cpp
 * Author: dmitri
 *
 * Created on 26 февраля 2017 г., 8:24
 */

#include <vector>
#include <thread>

#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <mutex>
#include <list>

#include "ICalculation.hpp"
#include "ThreadManager.hpp"
#include "Logger.hpp"

ThreadManager::ThreadManager() : isStopThreads_(false), isPaused_(false), isSomeOneUsedCout_(false), isReadyAcceptNewData_(true) {
    calculations_.reserve(1000);
    sizeOfCalculations_ = 0;
    numberOfThreads_ = 0;
}

ThreadManager::~ThreadManager() {
}

void
ThreadManager::start(size_t numberOfThreads) {
    threads_.create_thread(boost::bind(&ThreadManager::makeProcessThreadOrder, this));
    threads_.create_thread(boost::bind(&ThreadManager::updateAcceptStatus, this));

    numberOfThreads_ = numberOfThreads;

    for (size_t i = 0; i < numberOfThreads_; ++i) {
        processMap_.insert(std::make_pair(i, nullptr));

        if (i % 2 == 0) {
            threads_.create_thread(boost::bind(&ThreadManager::useMergeData, this, i));
        } else {
            threads_.create_thread(boost::bind(&ThreadManager::useCalculateData, this, i));
        }
    }
}

void
ThreadManager::stop() {
    isStopThreads_ = true;

    threads_.join_all();

    calculations_.clear();
    sizeOfCalculations_ = 0;
}

void
ThreadManager::addNewCalculation(ICalculation *calculation) {
    if (!isStopThreads_) {
        isPaused_ = true;

        {
            boost::unique_lock<boost::timed_mutex> lock(lockCalcutaions_, boost::try_to_lock);

            while(true) {
                if (lock) {
                    calculations_.push_back(calculation);
                    sizeOfCalculations_++;
                    break;
                } else {
                    std::this_thread::yield();
                    lock.try_lock_for(boost::chrono::milliseconds(1));
                }
            }
        }

        Logger::getInstance().info("ThreadManager::addNewCalculation");

        isPaused_ = false;
    }
}

void
ThreadManager::releaseCalculation(ICalculation *calculation) {
    isPaused_ = true;

    {
        boost::unique_lock<boost::timed_mutex> lock(lockCalcutaions_, boost::try_to_lock);

        while(true) {
            if (lock) {
                if (sizeOfCalculations_ > 0) {
                    bool found = false;

                    auto end = calculations_.end();
                    for (auto it = calculations_.begin(); it != end; ++it) {
                        if (*it == calculation) {
                            calculations_.erase(it);
                            found = true;

                            if (sizeOfCalculations_ > 0) {
                                sizeOfCalculations_--;
                            }

                            break;
                        }
                    }

                    if (found) {
                        for (auto it = processMap_.begin(); it != processMap_.end(); ++it) {
                            if ((*it).second != nullptr) {
                                boost::unique_lock<boost::timed_mutex> lockThread((*it).second->lockOrder);
                                (*it).second->order.clear();
                            }
                        }

                        Logger::getInstance().info("ThreadManager::releaseCalculation found ICalculation *calculation");
                    } else {
                        Logger::getInstance().error("ThreadManager::releaseCalculation not found ICalculation *calculation");
                    }
                }

                break;
            } else {
                std::this_thread::yield();
                lock.try_lock_for(boost::chrono::milliseconds(1));
            }
        }
    }

    isPaused_ = false;
}

void
ThreadManager::releaseAll() {
    isPaused_ = true;

    {
        boost::unique_lock<boost::timed_mutex> lock(lockCalcutaions_, boost::try_to_lock);

        while (true) {
            if (lock) {
                calculations_.erase(calculations_.begin(), calculations_.end());
                break;
            } else {
                std::this_thread::yield();
                lock.try_lock_for(boost::chrono::milliseconds(1));
            }
        }

        for (auto it = processMap_.begin(); it != processMap_.end(); ++it) {
            if ((*it).second != nullptr) {
                boost::unique_lock<boost::timed_mutex> lockThread((*it).second->lockOrder, boost::try_to_lock);

                while(true) {
                    if (lockThread) {
                        (*it).second->order.clear();
                        break;
                    } else {
                        std::this_thread::yield();
                        lockThread.try_lock_for(boost::chrono::milliseconds(1));
                    }
                }
            }
        }
    }

    sizeOfCalculations_ = 0;

    isPaused_ = false;
}

void
ThreadManager::useMergeData(size_t threadNumber) {
    threadCalculation tCalc;
    tCalc.order.reserve(1000);
    ICalculation *calc = nullptr;

    processMap_.at(threadNumber) = &tCalc;

    bool isEmpty = false;

    while (!isStopThreads_) {
        isEmpty = false;

        if (!isPaused_) {
            boost::unique_lock<boost::timed_mutex> lock(tCalc.lockOrder, boost::try_to_lock);

            while (true) {
                if (lock) {
                    if (!tCalc.order.empty()) {
                        calc = tCalc.order.front();

                        if (calc != nullptr) {
                            calc->mergeData();
                            calc->calculateData();
                            tCalc.order.erase(tCalc.order.begin());
//                            tCalc.order.pop_front();
                        } else {
                            Logger::getInstance().error("ThreadManager::useMergeData empty tCalc.order.front()");
                        }
                    } else {
                        isEmpty = true;
                    }

                    break;
                } else {
                    std::this_thread::yield();
                    lock.try_lock_for(boost::chrono::milliseconds(1));
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (isEmpty) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void
ThreadManager::useCalculateData(size_t threadNumber) {
    threadCalculation tCalc;
    tCalc.order.reserve(1000);
    ICalculation *calc = nullptr;

    processMap_.at(threadNumber) = &tCalc;
    bool isEmpty = false;

    while (!isStopThreads_) {
        isEmpty = false;

        if (!isPaused_) {
            boost::unique_lock<boost::timed_mutex> lock(tCalc.lockOrder, boost::try_to_lock);

            while (true) {
                if (lock) {
                    if (!tCalc.order.empty()) {
                        calc = tCalc.order.front();

                        if (calc != nullptr) {
                            calc->calculateData();
                            calc->mergeData();
                            tCalc.order.erase(tCalc.order.begin());
//                            tCalc.order.pop_front();
                        } else {
                            Logger::getInstance().error("ThreadManager::useCalculateData empty tCalc.order.front()");
                        }
                    } else {
                        isEmpty = true;
                    }

                    break;
                } else {
                    std::this_thread::yield();
                    lock.try_lock_for(boost::chrono::milliseconds(1));
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (isEmpty) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void
ThreadManager::makeProcessThreadOrder() {
    //should garantee that all elements in map not empty
    std::this_thread::sleep_for(std::chrono::seconds(2));

    while (!isStopThreads_) {
        if (!isPaused_) {
            {
                boost::unique_lock<boost::timed_mutex> lock(lockCalcutaions_, boost::try_to_lock);

                if (lock) {
                    for(auto it = processMap_.begin(); it != processMap_.end(); ++it){
                        if ((*it).second != nullptr) {
                            boost::unique_lock<boost::timed_mutex> lockThread((*it).second->lockOrder, boost::try_to_lock);

                            if (lockThread) {
                                if ((*it).second->order.empty()) {
                                    for(auto jt = calculations_.begin(); jt != calculations_.end(); ++jt) {
                                        (*it).second->order.push_back(*jt);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void
ThreadManager::someOneUsedCout() {
    isSomeOneUsedCout_ = true;
}

bool
ThreadManager::isReadyAcceptNewData() {
    return isReadyAcceptNewData_;
}

void
ThreadManager::updateAcceptStatusHandler(const boost::system::error_code& error)
{
    isReadyAcceptNewData_ = !isSomeOneUsedCout_;
    isSomeOneUsedCout_ = false;
}

void
ThreadManager::updateAcceptStatus() {
    while (!isStopThreads_) {
        boost::asio::io_service io;

        boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));
        t.async_wait(boost::bind(&ThreadManager::updateAcceptStatusHandler, this, boost::asio::placeholders::error));

        io.run();
    }
}
