/*
 * File:   GlobalConfiguration.hpp
 * Author: markov
 */

#ifndef GLOBALCONFIGURATION_HPP
#define	GLOBALCONFIGURATION_HPP

#include <string>
#include <atomic>

#include <boost/thread/mutex.hpp>

class GlobalConfiguration {
public:

    static GlobalConfiguration&
    getInstance() {
        static GlobalConfiguration currentInstance;
        return currentInstance;
    }
    bool isInitialized();
    std::string& getExeWorkPath();
    std::string getDBConectionString();
    std::string& getPlotExportPath();
    std::string& getRawLogPathForSave();
    std::string& getPathForIrregulationsOutput();

    double getReceiverX();
    double getReceiverY();
    double getReceiverZ();

    bool getIsReceiverXyzSet();

    void setReceiverXYZ(double x, double y, double z);

    unsigned short getTcpReceiverPort();
    unsigned short getTcpSenderPort();
private:
    std::atomic<bool> isErrorExist_;
    std::atomic<bool> isReceiverXyzSet_;

    std::string exeWorkPath_;

    struct DBConfig {
        std::string hostname;
        std::string username;
        std::string password;
        std::string dbname;
    };

    DBConfig dbConfig_;

    std::string plotExportPath_;
    std::string rawLogPathForSave_;
    std::string pathForIrregulationsOutput_;

    unsigned short tcpReceiverPort_;
    unsigned short tcpSenderPort_;

    boost::mutex mutexLockReceiverXyz_;

    double receiverX_;
    double receiverY_;
    double receiverZ_;

    GlobalConfiguration();
    GlobalConfiguration(const GlobalConfiguration& orig) = delete;
    GlobalConfiguration& operator=(const GlobalConfiguration& ) = delete;
    virtual ~GlobalConfiguration();
    void readConfigurationFile(std::string& filename);
};

#endif	/* GLOBALCONFIGURATION_HPP */

