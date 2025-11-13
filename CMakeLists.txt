cmake_minimum_required(VERSION 3.10)
project(EvolutionSimulation)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# if(CMAKE_EXPORT_COMPILE_COMMANDS)
#     set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
# endif()

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
add_executable(test test.cc)
target_link_libraries(test PRIVATE sfml-graphics sfml-window sfml-system)
