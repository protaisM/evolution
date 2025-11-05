cmake_minimum_required(VERSION 3.10)
project(EvolutionSimulation)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# if(CMAKE_EXPORT_COMPILE_COMMANDS)
#     set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
# endif()

add_executable(test test.cc)
