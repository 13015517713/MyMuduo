cmake_minimum_required(VERSION 2.8)

project(MyMuduo)


set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib")
# 设置依赖路径

AUX_SOURCE_DIRECTORY(. SOURCES)
add_library(muduo SHARED ${SOURCES})

# add_executable(test ${SOURCES})

