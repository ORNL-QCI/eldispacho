cmake_minimum_required(VERSION 2.8)
project(eldispacho)

########################################################################
# BUILD SETUP
########################################################################
option(USE_STATIC "Build with static libraries" OFF)
option(BUILD_RELEASE "Build for release" OFF)

########################################################################
# SOURCES
########################################################################
set(eldispacho_sources
	simulator/unit.cpp
	simulator/adapter.cpp
	simulator/client.cpp
	simulator/client_pool.cpp
	model/interface.cpp
	model/network/node.cpp
	model/network/endpoint.cpp
	model/network/qswitch.cpp
	model/network/circulator_switch.cpp
	model/network/system.cpp
	net/server.cpp
	processor.cpp
	diagnostics/logger.cpp
	diagnostics/server.cpp
	eldispacho.cpp)

########################################################################
# CONFIGURE BUILD TYPE
########################################################################
message("------------------------------")
message("Build Configuration:")

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -pedantic")
endif()

if(BUILD_RELEASE)
	# Release build strips assert statements and automatically enables
	# compiler optimizations.
	add_definitions(-DNDEBUG)
	if(CMAKE_COMPILER_IS_GNUCXX)
		# CMake doesn't automatically strip
		set(CMAKE_EXE_LINKER_FLAGS "-s")
	endif()
	# This value is stored in the cache!
	set(CMAKE_BUILD_TYPE Release)
	message("Building for release")
else()
	# Debug build keeps assert statements and automatically disables
	# any compiler optimizations.
	# This value is stored in the cache!
	set(CMAKE_BUILD_TYPE Debug)
	message("Building for debug")
endif()

if(USE_STATIC)
	message("Using static libaries")
	message("ZMQ probably won't be static...")
	set(Boost_USE_STATIC_LIBS ON)
	set(Boost_USE_STATIC_RUNTIME ON)
	find_library(ZMQ_LIB zmq REQUIRED)
else()
	message("Using shared libraries")
	set(Boost_USE_STATIC_LIBS OFF)
	set(Boost_USE_STATIC_RUNTIME OFF)
	find_library(ZMQ_LIB zmq REQUIRED)
endif()

find_package(Boost COMPONENTS system thread program_options date_time REQUIRED)
link_directories(${Boost_LIBRARY_DIR})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/tpls/cppzmq/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/tpls/rapidjson/include/")

message("------------------------------")

########################################################################
# BUILD
########################################################################
add_executable(eldispacho ${eldispacho_sources})
target_link_libraries(eldispacho pthread ${Boost_LIBRARIES} ${ZMQ_LIB})

if(CMAKE_MAJOR_VERSION GREATER 2 AND CMAKE_MINOR_VERSION GREATER 0)
	target_compile_features(eldispacho PRIVATE cxx_range_for)
	target_compile_features(eldispacho PRIVATE cxx_range_for)
else()
	if(CMAKE_COMPILER_IS_GNUCXX)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	else()
		message(FATAL_ERROR "Your CMake is old and you don't have GCC")
	endif()
endif()

########################################################################
# INSTALL
########################################################################
install(TARGETS eldispacho DESTINATION bin)

########################################################################
# UNINSTALL
########################################################################
#add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake)
