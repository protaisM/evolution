add_executable(level1 test.cc)
add_executable(test_brain test_brain.cc)

target_link_libraries(level1
    PUBLIC
        map
        global
        players
        graphics
        neural_network
        predators
        sfml-graphics
        sfml-window
        sfml-system
    )

target_link_libraries(test_brain
    PUBLIC
        neural_network
    )

