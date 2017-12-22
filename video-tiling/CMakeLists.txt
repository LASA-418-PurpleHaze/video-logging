
cmake_minimum_required(VERSION 3.2)
project( video-tiling )
find_package( OpenCV REQUIRED )
set(SOURCE_FILES main.cpp)
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable(render render.cpp)
target_link_libraries( render ${OpenCV_LIBS} )
