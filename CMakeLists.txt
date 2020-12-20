cmake_minimum_required(VERSION 2.8)

project(MuduoUse2)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
# 设置依赖路径
set(DELY "${PROJECT_SOURCE_DIR}/../build/release-cpp11")
set(SOURCES EchoServer.cpp)

add_executable(test ${SOURCES})

#  下面两个都可以添加头文件搜索库  当然第二个也可以   更喜欢第一个

# include_directories(${PROJECT_SOURCE_DIR})
# target_include_directories(test  
#     PUBLIC 
#     ${DELY}/include
# )
#   下面添加链接库路径，然后链接进来会多去几个路径  这个应该没什么问题
link_directories(${DELY}/lib)  
target_link_libraries(test
    PUBLIC muduo_net muduo_base  pthread
)
include_directories(${DELY}/include)
