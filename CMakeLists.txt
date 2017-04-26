cmake_minimum_required(VERSION 3.5)

project(ionozondpostprocessing)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

FIND_PACKAGE( Boost 1.58 COMPONENTS program_options thread filesystem REQUIRED )

include_directories(${Boost_INCLUDE_DIRS})

include_directories(src)
include_directories(src/common)
include_directories(src/logger)
include_directories(src/satellite)
include_directories(src/parser)
include_directories(src/thread_manager)
include_directories(src/calculations)
include_directories(src/experiments)

add_executable(ionozondpostprocessing src/common/common_functions.cpp
					src/main.cpp
					src/common/crc32.cpp
					src/calculations/PashintsevCalculation.cpp
					src/calculations/DoubleChannelCalculations.cpp
					src/calculations/pashintsev_const.cpp
					src/common/GlobalConfiguration.cpp
					src/calculations/PashitsevSatelliteChannel.cpp
					src/satellite/GlonassSatelliteChannel.cpp
					src/common/PlotData.cpp
					src/satellite/GlonassSatellite.cpp
					src/experiments/PredictionsExperiments.cpp
					src/common/gnss_const.cpp
					src/calculations/S4Calculation.cpp
					src/common/gnss_functions.cpp
					src/calculations/S4IrregulationsSearch.cpp
					src/common/GnssLogTime.cpp
					src/satellite/SatelliteChannel.cpp
					src/satellite/GpsSatelliteChannel.cpp
					src/satellite/Satellite.cpp
					src/satellite/GpsSatellite.cpp
					src/satellite/SatelliteGroup.cpp
					src/common/server_consts.cpp
					src/experiments/IonoZondExperiment16.cpp
					src/calculations/SigmaPhiCalculation.cpp
					src/experiments/IonoZondExperiment17.cpp
					src/experiments/SvmPredictionData.cpp
					src/logger/Logger.cpp
					src/thread_manager/ThreadManager.cpp
					src/parser/LogParserThread.cpp
                                        src/experiments/IonoZondExperiment19.cpp
                                        src/experiments/IonoZondExperiment20.cpp
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -m64 -DDEBUG -march=haswell")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -m64 -march=native -Ofast")

set_property(TARGET ionozondpostprocessing PROPERTY CXX_STANDARD 14)

target_link_libraries(ionozondpostprocessing ${Boost_LIBRARIES})
