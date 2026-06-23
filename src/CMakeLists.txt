add_library(src STATIC predator.cpp)
target_include_directories(src
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})
