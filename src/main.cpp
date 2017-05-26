/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: dmitri
 *
 * Created on 11 Июнь 2016 г., 13:54
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <chrono>

#include <boost/program_options.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <signal.h>

#include "LogParserThread.hpp"
#include "SvmPredictionData.hpp"
#include "server_consts.hpp"
#include "common_functions.hpp"
#include "ThreadManager.hpp"

//const int MAX_BUFFER_SIZE = 33554432;
static std::string TMP_UNPACK_DIR = "";

int
unpack_archive(const char* file_name) {
    //tar -xf archive.tar -C /target/directory
    std::stringstream command;
    command << "tar --skip-old-files -xf ";
    command << file_name;
    command << " -C ";
    command << TMP_UNPACK_DIR;

    FILE* pd = popen(command.str().c_str(), "r");
    if (pd == NULL) {
        perror("popen");
        exit(1);
    } else {
        char buf[1024];
        size_t bytes = fread(buf, 1, 1023, pd);
        std::cerr << command.str() << " " << bytes << std::endl;
    }

    int ret = pclose(pd);
    return WEXITSTATUS(ret);
}

int
delete_file(const char* file_name) {
    //tar -xf archive.tar -C /target/directory
    std::stringstream command;
    command << "rm -f ";
    command << file_name;
    FILE* pd = popen(command.str().c_str(), "r");
    if (pd == NULL) {
        perror("popen");
        exit(1);
    } else {
        char buf[1024];
        size_t bytes = fread(buf, 1, 1023, pd);
        std::cerr << command.str() << " " << bytes << std::endl;
    }

    int ret = pclose(pd);
    return WEXITSTATUS(ret);
}

int
create_dir_recursive(const char* directory) {
    std::stringstream command;
    command << "mkdir -p ";
    command << directory;
    FILE* pd = popen(command.str().c_str(), "r");
    if (pd == NULL) {
        perror("popen");
        exit(1);
    } else {
        char buf[1024];
        size_t bytes = fread(buf, 1, 1023, pd);
        std::cerr << command.str() << " " << bytes << std::endl;
    }

    int ret = pclose(pd);
    return WEXITSTATUS(ret);
}

void option_dependency(const boost::program_options::variables_map& vm,
                        const char* for_what, const char* required_option)
{
    if (vm.count(for_what) > 0 && !vm[for_what].defaulted())
        if (vm.count(required_option) == 0 || vm[required_option].defaulted())
            throw std::logic_error(std::string("Option '") + for_what
                              + "' requires option '" + required_option + "'.");
}

static std::atomic<bool> isStopAll(false);

void
signalTerminate(int signum) {
        isStopAll = true;
        ThreadManager::getInstance().stop();
        std::this_thread::yield();
        SvmPredictionData::getInstance().stop();
        std::this_thread::yield();
}

int
main(int argc, char *argv[]) {

//    std::this_thread::sleep_for (std::chrono::seconds(30));

    signal(SIGINT, signalTerminate);
    signal(SIGTERM, signalTerminate);

    std::string fileWithSources = "";
    std::string nameOfExperiment = "";
    std::string nameOfOutputSatelliteSystem = "";
    std::string idOfOutputSatellite = "";
    std::string nameOfSatelliteChannel = "";

    size_t timeToSleep = 10;
    size_t experiment21shift = 3000;

    boost::program_options::options_description desc("Usage options");
    desc.add_options()
            // First parameter describes option name/short name
            // The second is parameter to option
            // The third is description
            ("help,h", "print usage message")
            ("file-with-sources", boost::program_options::value(&fileWithSources), "file that contains list of packed RAW_LOG_###.tar.gz")
            ("experiment", boost::program_options::value(&nameOfExperiment), "name of experiment that will be activated")
            ("tmp-dir", boost::program_options::value(&PATH_OUT_TMP_MERGE_DATA), "folder that will be used for caching of files")
            ("tmp-unpack-dir", boost::program_options::value(&TMP_UNPACK_DIR), "folder that will be used for temporary unpacking of files")
            ("merge-output-dir", boost::program_options::value(&PATH_OUT_MERGE_DATA), "folder that will be used for output of mergedata in csv format")
            ("satellite-system", boost::program_options::value(&nameOfOutputSatelliteSystem), "Name of satellite system that should be output (GPS/GLONASS)")
            ("satellite-id", boost::program_options::value(&idOfOutputSatellite), "ID of satellite that will be output GPS(1-32), GLONASS(1-24)")
            ("satellite-channel", boost::program_options::value(&nameOfSatelliteChannel), "channel L1, L2")
            ("sleep", boost::program_options::value(&timeToSleep), "Delay between reads from stdin in milliseconds")
			("experiment21shift", boost::program_options::value(&experiment21shift), "The shift state used in 21 experiment")
            ;

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if (vm.empty() || vm.count("help")) {
        std::cerr << desc << "\n";
        return 0;
    }

    option_dependency(vm, "experiment", "tmp-dir");
    option_dependency(vm, "tmp-dir", "tmp-unpack-dir");
    option_dependency(vm, "tmp-unpack-dir", "merge-output-dir");

//    ThreadManager::getInstance().start(24);
    ThreadManager::getInstance().start(4);

    SvmPredictionData::getInstance().start(nameOfExperiment, nameOfOutputSatelliteSystem, idOfOutputSatellite, nameOfSatelliteChannel);

    {
        LogParserThread parser;
        create_dir_recursive(PATH_OUT_TMP_MERGE_DATA.c_str());

        if (!fileWithSources.empty()) {
            std::ifstream infile(fileWithSources, std::ios_base::in);
            std::string archiveOfRawLogs = "";
            while (infile >> archiveOfRawLogs  && !isStopAll) {
                std::string filename = archiveOfRawLogs;
                filename = filename.substr(0, filename.find_last_of(".tar.gz") - 6);
                filename = filename.substr(filename.find_last_of("/") + 1, filename.length() - filename.find_last_of("/"));
                filename = TMP_UNPACK_DIR + "/" + filename;

                if (unpack_archive(archiveOfRawLogs.c_str()) == 0) {
                    //первый архив начинаем обрабатывать
                    std::fstream inputFile;
                    inputFile.open(filename.c_str(), std::ios::binary | std::ios::in);

                    if (inputFile.is_open()) {
                        int FBytesRead = 1;
                        while (!inputFile.eof() && (FBytesRead != 0) && !isStopAll) {
                            char *FIOBuffer = new char[BinaryRawData::MAX_DATA_SIZE];
                            inputFile.read(FIOBuffer, BinaryRawData::MAX_DATA_SIZE);

                            FBytesRead = inputFile.gcount();

                            if (FBytesRead == 0) {
                                std::cerr << "FInputFile.fail() ==" << inputFile.fail() << std::endl;
                                std::cerr << "FInputFile.bad() ==" << inputFile.bad() << std::endl;
                                break;
                            }

                            parser.pushRawData(reinterpret_cast<unsigned char*> (FIOBuffer), FBytesRead);
    #ifdef DEBUG
//                            std::this_thread::yield();
                            std::this_thread::sleep_for (std::chrono::milliseconds(2));
    #else
                            std::this_thread::sleep_for (std::chrono::milliseconds(2));
//                            std::this_thread::yield();
    #endif
                        }

                        inputFile.close();
                    }
                }

                delete_file(filename.c_str());
            }
        } else {
            int FBytesRead = 1;
            int64_t wasProcessedBytes = 0;
            unsigned char *iBuffer = new unsigned char[BinaryRawData::MAX_DATA_SIZE];

            while (!std::cin.eof() && (FBytesRead != 0) && !isStopAll) {
                if (ThreadManager::getInstance().isReadyAcceptNewData()) {

                    std::cin.read((char*)iBuffer, BinaryRawData::MAX_DATA_SIZE);
                    FBytesRead = std::cin.gcount();

                    if (FBytesRead == 0) {
                        std::cerr << "std::cin.fail() ==" << std::cin.fail() << std::endl;
                        std::cerr << "std::cin.bad() ==" << std::cin.bad() << std::endl;
                        break;
                    }

                    parser.pushRawData(iBuffer, FBytesRead);
                    wasProcessedBytes += FBytesRead;
                }

#ifdef DEBUG
//                std::this_thread::yield();
                Logger::getInstance().info("pushRawData " + intToString(FBytesRead) + " bytes. Processed " +  doubleToString ((long double)wasProcessedBytes / 1024.0 / 1024.0 /1024.0, 2) + "GB");
                std::this_thread::sleep_for (std::chrono::milliseconds(timeToSleep));
#else
                std::this_thread::sleep_for (std::chrono::milliseconds(timeToSleep));
#endif
            }

            delete[] iBuffer;

#ifdef DEBUG
                Logger::getInstance().info("File processed. Main loop exit.");
#endif

            std::this_thread::sleep_for (std::chrono::minutes(1));
        }
    }

    SvmPredictionData::getInstance().stop();
    ThreadManager::getInstance().stop();

    return 0;
}
