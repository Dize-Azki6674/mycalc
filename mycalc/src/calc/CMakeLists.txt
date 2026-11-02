add_library(calc
    parser/builtin/sequential.cpp
)

target_include_directories(calc PUBLIC
    "${PROJECT_SOURCE_DIR}/mycalc/include"
)

target_link_libraries(calc PUBLIC
    Boost::rational
)