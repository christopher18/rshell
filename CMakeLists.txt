cmake_minimum_required(VERSION 3.6)
project(rshell)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES client.cpp server.cpp ShellSocket.cpp ShellSocket.h)
add_executable(rshell ${SOURCE_FILES})