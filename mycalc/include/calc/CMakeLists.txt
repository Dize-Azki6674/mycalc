# include/calc/CMakeLists.txt

add_library(calc STATIC
    # Public headers
    ${CMAKE_CURRENT_SOURCE_DIR}/calc.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/formula/subformula.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/parser.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parser/builtin/sequential.hpp

    # Sources
    ${PROJECT_SOURCE_DIR}/mycalc/src/calc/parser/builtin/sequential.cpp
)

target_include_directories(calc
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_compile_features(calc
    PUBLIC
        cxx_std_23
)

target_link_libraries(calc
    PUBLIC
        Boost::rational
)