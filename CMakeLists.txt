cmake_minimum_required(VERSION 3.28)
project(EvolutionSimulation)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# if(CMAKE_EXPORT_COMPILE_COMMANDS)
#     set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
# endif()
add_compile_options(-Wall -Wpedantic -Wextra -g)

add_subdirectory(src)
add_subdirectory(tests)

# json library
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(nlohmann_json)

# sfml
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

