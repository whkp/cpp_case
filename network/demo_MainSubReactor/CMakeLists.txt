cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS ON)

project(Pine)
find_package(Threads REQUIRED)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/src/include)
add_subdirectory(src)

add_executable(echo_server test/echo_server.cpp)
add_executable(echo_client test/echo_client.cpp)

target_link_libraries(echo_server network )
target_link_libraries(echo_client network pthread)