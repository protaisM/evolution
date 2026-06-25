add_library(predators STATIC predator.cpp)

target_include_directories(predators
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(predators PUBLIC map)
