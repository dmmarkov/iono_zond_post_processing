#include "common_functions.hpp"
#ifdef DEBUG
#include "Logger.hpp"
#endif
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <iomanip>
#include <sstream>
#include <sys/mman.h>

boost::mutex pTimeLock_;
boost::mutex hugePagesLock_;
boost::mutex mutexLockCout_;

bool
isFileExists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

std::string
doubleToString(double value, int precision) {
/*TODO optimize this function*/
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(precision);
    ss << value;
    return ss.str();
}

std::string
intToString(int value) {
    char res[25] = {'\0'};
    snprintf(res, sizeof(res), "%d", value);
    return std::string(res);
}

std::string
intToString(long int value) {
    char res[25] = {'\0'};
    snprintf(res, sizeof(res), "%ld", value);
    return std::string(res);
}

std::string
intToString(unsigned int value) {
    char res[25] = {'\0'};
    snprintf(res, sizeof(res), "%u", value);
    return std::string(res);
}

std::string
intToString(unsigned long int value) {
    char res[25] = {'\0'};
    snprintf(res, sizeof(res), "%lu", value);
    return std::string(res);
}

std::string
pTimeToSimpleString(boost::posix_time::ptime& time) {
    boost::unique_lock<boost::mutex> lock(pTimeLock_);

    return boost::posix_time::to_simple_string(time);
}

std::string
pTimeToIsoString(boost::posix_time::ptime& time) {
    boost::unique_lock<boost::mutex> lock(pTimeLock_);

    return boost::posix_time::to_iso_extended_string(time);
}

 #define PROTECTION (PROT_READ | PROT_WRITE)

 #ifndef MAP_HUGETLB
 #define MAP_HUGETLB 0x40000 /* arch specific */
 #endif

 /* Only ia64 requires this */
 #ifdef __ia64__
 #define ADDR (void *)(0x8000000000000000UL)
 #define FLAGS (MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB | MAP_FIXED)
 #else
 #define ADDR (void *)(0x0UL)
 #define FLAGS (MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB)
 #endif

 #define MIN_LENGTH (2UL*1024*1024)

void *
allocateHugePages(size_t *sizeOfMemory) {
    boost::unique_lock<boost::mutex> lock(hugePagesLock_);

    size_t realSizeOfMemory = ((*sizeOfMemory) / MIN_LENGTH) * MIN_LENGTH + (!!((*sizeOfMemory) % MIN_LENGTH)) * MIN_LENGTH;
#ifdef DEBUG
    Logger::getInstance().info("Allocate hugepages " + intToString(*sizeOfMemory));
#endif
    void *addr = mmap(ADDR, realSizeOfMemory, PROTECTION, FLAGS, 0, 0);

    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    *sizeOfMemory = realSizeOfMemory;

    return addr;
}

int
freeHugePages(void *addr, size_t sizeOfMemory) {
    boost::unique_lock<boost::mutex> lock(hugePagesLock_);

    if (munmap(addr, sizeOfMemory)) {
        perror("munmap");
        exit(1);
    }

    return 0;
}

void
printCout(const char *source) {
    boost::unique_lock<boost::mutex> lock(mutexLockCout_);

    std::cout << *source << "\n";
}

void
printCout(const std::string source) {
    boost::unique_lock<boost::mutex> lock(mutexLockCout_);

    std::cout << source << "\n";
}

void
printCout(std::__cxx11::basic_stringstream<char>::__stringbuf_type* source) {
    boost::unique_lock<boost::mutex> lock(mutexLockCout_);

    std::cout << source << "\n";
}