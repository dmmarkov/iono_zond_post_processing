cmake_minimum_required(VERSION 3.5)

project(ionozondpostprocessing)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 

FIND_PACKAGE( Boost 1.58 COMPONENTS program_options thread filesystem REQUIRED )

include_directories(${Boost_INCLUDE_DIRS}) 

add_executable(ionozondpostprocessing src/common_functions.cpp 
					src/main.cpp 
					src/crc32.cpp
					src/PashintsevCalculation.cpp
					src/DoubleChannelCalculations.cpp
					src/pashintsev_const.cpp
					src/GlobalConfiguration.cpp
					src/PashitsevSatelliteChannel.cpp
					src/GlonassSatelliteChannel.cpp
					src/PlotData.cpp
					src/GlonassSatellite.cpp
					src/PredictionsExperiments.cpp
					src/gnss_const.cpp
					src/S4Calculation.cpp
					src/gnss_functions.cpp
					src/S4IrregulationsSearch.cpp
					src/GnssLogTime.cpp
					src/SatelliteChannel.cpp
					src/GpsSatelliteChannel.cpp
					src/Satellite.cpp
					src/GpsSatellite.cpp
					src/SatelliteGroup.cpp
					src/server_consts.cpp
					src/IonoZondExperiment16.cpp
					src/SigmaPhiCalculation.cpp
					src/IonoZondExperiment17.cpp
					src/SvmPredictionData.cpp
					src/Logger.cpp
					src/ThreadManager.cpp
					src/LogParserThread.cpp)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -m64 -DDEBUG -march=haswell")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -m64 -march=native -Ofast")

set_property(TARGET ionozondpostprocessing PROPERTY CXX_STANDARD 14)

target_link_libraries(ionozondpostprocessing ${Boost_LIBRARIES})
