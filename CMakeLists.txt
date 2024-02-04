cmake_minimum_required(VERSION 3.22)
project(cpp-common)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "-rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

set(LIB_SRC
    common/log.cpp)

add_library(common SHARED ${LIB_SRC})

add_executable(test tests/test.cc)

add_dependencies(test common)
target_link_libraries(test common)


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/lib)