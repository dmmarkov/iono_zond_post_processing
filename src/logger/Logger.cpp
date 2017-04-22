/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Logger.cpp
 * Author: markov
 */

#include <fstream>
#include <ctime>
#include <thread>

#include "Logger.hpp"
#include "server_consts.hpp"
#include "common_functions.hpp"

Logger::Logger() : isErrorExist_(false) {
    loggerFile_.rdbuf()->pubsetbuf(0, 0);
    loggerFile_.open(FILENAME_LOG, std::ios_base::out | std::ios_base::app);
    if (loggerFile_.is_open()) {
        info("Log file opened succesfull");
    } else {
        isErrorExist_ = true;
    }
}

Logger::~Logger() {
    loggerFile_.close();
}

std::string
Logger::formatMessage(std::string errorType, std::string message) {
    std::ostringstream ss;
    ss << "[" << getCurrentTimeStamp() << "] : ";
    ss << errorType << " ";
    ss << std::this_thread::get_id() << " ";
    ss << message << std::endl;
    return ss.str();
}

void
Logger::error(const char* message) {
    writeMessage("ERROR", std::string(message));
}

void
Logger::error(std::string message) {
    writeMessage("ERROR", message);
}

void
Logger::info(std::string message) {
    writeMessage("INFO", message);
}

void
Logger::writeMessage(std::string errorType, std::string message) {
    boost::unique_lock<boost::mutex> lock(mutexWriteLog_);

    std::string fullMessage = formatMessage(errorType, std::string(message));
    loggerFile_.write(fullMessage.c_str(), fullMessage.length());
}

bool
Logger::init() {
    if (!isErrorExist_) {
        info("Logger initialized");
        return true;
    } else {
        return false;
    }
}

std::string
Logger::getCurrentTimeStamp() {
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

    return pTimeToSimpleString(now);
}
