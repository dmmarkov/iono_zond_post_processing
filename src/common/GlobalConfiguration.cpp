/*
 * File:   GlobalConfiguration.cpp
 * Author: markov
 */

#include "GlobalConfiguration.hpp"
#include "Logger.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

GlobalConfiguration::GlobalConfiguration() : isErrorExist_(false),
                                             isReceiverXyzSet_(false),
                                             exeWorkPath_(boost::filesystem::current_path().string())
{
    tcpReceiverPort_ = 0;
    tcpSenderPort_ = 0;

    boost::filesystem::path ConfigPath(exeWorkPath_);
    ConfigPath += "/config.ini";
    std::string ConfigFileName = ConfigPath.string();

    readConfigurationFile(ConfigFileName);
}

GlobalConfiguration::~GlobalConfiguration() {

}

std::string&
GlobalConfiguration::getExeWorkPath() {
    return exeWorkPath_;
}

std::string
GlobalConfiguration::getDBConectionString() {
    return "host=" + dbConfig_.hostname +
            " user=" + dbConfig_.username +
            " password=" + dbConfig_.password +
            " dbname=" + dbConfig_.dbname;
}

std::string&
GlobalConfiguration::getPlotExportPath() {
    return plotExportPath_;
}

void
GlobalConfiguration::readConfigurationFile(std::string& filename) {
    boost::property_tree::ptree pt;
    try {
        boost::property_tree::ini_parser::read_ini(filename, pt);

        dbConfig_.hostname = pt.get<std::string>("database.host", "any_host");
        dbConfig_.username = pt.get<std::string>("database.user", "any_user");
        dbConfig_.password = pt.get<std::string>("database.password", "any_password");
        dbConfig_.dbname = pt.get<std::string>("database.dbname", "any_dbname");

        plotExportPath_ = pt.get<std::string>("plot.export_path");

        rawLogPathForSave_ = pt.get<std::string>("raw_data.path_for_save");

        tcpReceiverPort_ = pt.get<unsigned short>("tcp_server.receiver_port", 12000);
        tcpSenderPort_ = pt.get<unsigned short>("tcp_server.sender_port", 12001);

        receiverX_ = pt.get<double>("receiver.x", 0.0);
        receiverY_ = pt.get<double>("receiver.y", 0.0);
        receiverZ_ = pt.get<double>("receiver.z", 0.0);

        isReceiverXyzSet_ = (receiverX_ + receiverY_ + receiverZ_) > 1;

        pathForIrregulationsOutput_ = pt.get<std::string>("irregulations.path_for_save");
    }
    catch (const std::exception& e) {
        Logger::getInstance().error(e.what());
        isErrorExist_ = true;
    }
}

bool
GlobalConfiguration::isInitialized() {
    return !isErrorExist_;
}

std::string&
GlobalConfiguration::getRawLogPathForSave() {
    return rawLogPathForSave_;
}

std::string&
GlobalConfiguration::getPathForIrregulationsOutput() {
    return pathForIrregulationsOutput_;
}

double
GlobalConfiguration::getReceiverX() {
    return receiverX_;
}

double
GlobalConfiguration::getReceiverY() {
    return receiverY_;
}

double
GlobalConfiguration::getReceiverZ() {
    return receiverZ_;
}

bool
GlobalConfiguration::getIsReceiverXyzSet() {
    return isReceiverXyzSet_;
}

void
GlobalConfiguration::setReceiverXYZ(double x, double y, double z) {
    if (!isReceiverXyzSet_) {
        boost::unique_lock<boost::mutex> lock(mutexLockReceiverXyz_);

        receiverX_ = x;
        receiverY_ = y;
        receiverZ_ = z;

        isReceiverXyzSet_ = true;
    } else {
        Logger::getInstance().error("The receiver coordinates was set erlier. It can not be change after it was set.");
    }
}

unsigned short
GlobalConfiguration::getTcpReceiverPort() {
    return tcpReceiverPort_;
}

unsigned short
GlobalConfiguration::getTcpSenderPort() {
    return tcpSenderPort_;
}