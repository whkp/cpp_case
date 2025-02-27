cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)

project(Pine)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/include)

add_executable(echo_server test/echo_server.cpp)
add_executable(echo_client test/echo_client.cpp)
