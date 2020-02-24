cmake_minimum_required(VERSION 3.14)
project(SonnenAufUntergang)

set(CMAKE_CXX_STANDARD 14)
include_directories(../arduino_lib/SunRiseSet)

add_executable(SonnenAufUntergang main.cpp ../arduino_lib/SunRiseSet/SunRiseSet.cpp ../arduino_lib/SunRiseSet/SunRiseSet.h)