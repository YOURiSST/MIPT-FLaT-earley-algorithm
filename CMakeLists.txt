cmake_minimum_required(VERSION 3.26)
project(MIPT_FLaT_earley_algorithm)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(MIPT_FLaT_earley_algorithm main.cpp earley_algorithm.cpp)
