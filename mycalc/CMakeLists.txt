# CMakeList.txt : mycalc の CMake プロジェクト。ソースを含めて、次を定義します:
# プロジェクト専用ロジックはこちらです。
#

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

find_package(OpenGL REQUIRED)
find_package(boost_static_string CONFIG REQUIRED)
find_package(boost_rational CONFIG REQUIRED)

add_subdirectory(external/glfw)

add_subdirectory("src")
add_subdirectory("test")
    