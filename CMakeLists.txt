cmake_minimum_required(VERSION 3.10)
project(EvolutionSimulation)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# if(CMAKE_EXPORT_COMPILE_COMMANDS)
#     set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
# endif()
add_compile_options(-Wall -Wpedantic -Wextra)

# json library
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(nlohmann_json)

# sfml
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(test test.cc)

# link all the libraries
target_link_libraries(test PRIVATE sfml-graphics sfml-window sfml-system)
target_link_libraries(test PRIVATE nlohmann_json::nlohmann_json)
