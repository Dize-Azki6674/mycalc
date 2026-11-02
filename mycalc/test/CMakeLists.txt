if(NOT BUILD_TESTING)
    return()
endif()

add_executable(test_seq_calc
    test_seq_calc.cpp
)

target_link_libraries(test_seq_calc PRIVATE
    calc
)

add_test(
    NAME seq_calc
    COMMAND test_seq_calc
)

add_executable(test_manager
    test_manager.cpp
)

target_link_libraries(test_manager PRIVATE
    gui
)

add_test(
    NAME manager
    COMMAND test_manager
)