add_library(gui
    calc/manager.cpp
)

target_include_directories(gui PUBLIC
    "${PROJECT_SOURCE_DIR}/mycalc/include"
)

target_link_libraries(gui PUBLIC
    calc
    Boost::rational
    Boost::static_string
)