cmake_minimum_required(VERSION 3.22)
project(cpp-common)

set(CMAKE_CXX_STANDARD 14)

add_executable(cpp-common
        common/log.cpp
        test/test.cc)