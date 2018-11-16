cmake_minimum_required(VERSION 3.12)
project(rainfall)

set(CMAKE_CXX_STANDARD 11)

add_executable(rainfall main.cpp Grid.h Node.h utility.cpp utility.h simulation.cpp simulation.h)