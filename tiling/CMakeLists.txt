
cmake_minimum_required(VERSION 3.2)
project( tilingtemplate )
find_package( OpenCV REQUIRED )
set(SOURCE_FILES main.cpp)
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable(tilingtemplate tilingtemplate.cpp)
target_link_libraries( tilingtemplate ${OpenCV_LIBS} )
