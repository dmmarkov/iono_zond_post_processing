/*
 * File:   common_functions.hpp
 * Author: dmitri
 */

#ifndef COMMON_FUNCTIONS_HPP
#define	COMMON_FUNCTIONS_HPP

#include "project_defines.hpp"

#include <iostream>
#include <string>

#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

extern boost::mutex pTimeLock_;
extern boost::mutex hugePagesLock_;
extern boost::mutex mutexLockCout_;

bool isFileExists(const std::string& name);
std::string doubleToString(double value, int precision);

std::string intToString(int value);
std::string intToString(long int value);
std::string intToString(unsigned int value);
std::string intToString(unsigned long int value);

std::string pTimeToSimpleString(boost::posix_time::ptime& time);
std::string pTimeToIsoString(boost::posix_time::ptime& time);

void *allocateHugePages(size_t *sizeOfMemory);
int freeHugePages(void *addr, size_t sizeOfMemory);

void printCout(const char *source);
void printCout(const std::string source);
void printCout(std::__cxx11::basic_stringstream<char>::__stringbuf_type* source);

#endif	/* COMMON_FUNCTIONS_HPP */

