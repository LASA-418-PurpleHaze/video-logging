cmake_minimum_required(VERSION 3.2)
project(receive_from_rio_template)
set(CMAKE_CXX_STANDARD 14)
find_package(PkgConfig)
pkg_check_modules(PC_ZeroMQ QUIET zmq)
find_path(ZeroMQ_INCLUDE_DIR
	NAMES zmq.hpp
	PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
	)
find_library(ZeroMQ_LIBRARY
	NAMES zmq
	PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
	)
set(SOURCE_FILES logging_server.cpp)
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable(logging_server ${SOURCE_FILES})
add_executable(receive_from_rio_template receive_from_rio_template.cpp)
add_executable(logging_client_demo logging_client_demo.cpp)
target_link_libraries( logging_server
				   ${ZeroMQ_LIBRARY} )
target_link_libraries( receive_from_rio_template
				   ${ZeroMQ_LIBRARY} )
target_link_libraries( logging_client_demo
				   ${ZeroMQ_LIBRARY} )
