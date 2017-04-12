/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Logger.hpp
 * Author: markov
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "project_defines.hpp"

#include <string>
#include <fstream>
#include <atomic>

#include <boost/thread/mutex.hpp>

class Logger {
public:
    static Logger&
    getInstance() {
        static Logger currentInstance;
        return currentInstance;
    }

    void error(const char* message);
    void error(std::string message);
    void info(std::string message);
    bool init();

    virtual ~Logger();
private:
    std::ofstream loggerFile_;
    std::atomic<bool> isErrorExist_;

    boost::mutex mutexWriteLog_;

    Logger();
    Logger(const Logger& orig) = delete;
    Logger& operator=(const Logger& ) = delete;

    std::string getCurrentTimeStamp();
    std::string formatMessage(std::string errorType, std::string message);
    void writeMessage(std::string errorType, std::string message);
};

#endif /* LOGGER_HPP */

