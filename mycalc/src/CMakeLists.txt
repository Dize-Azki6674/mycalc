add_subdirectory(calc)
add_subdirectory(gui)

add_executable(mycalc
    mycalc.cpp

    "${PROJECT_SOURCE_DIR}/mycalc/external/imgui/imgui.cpp"
    "${PROJECT_SOURCE_DIR}/mycalc/external/imgui/imgui_draw.cpp"
    "${PROJECT_SOURCE_DIR}/mycalc/external/imgui/imgui_tables.cpp"
    "${PROJECT_SOURCE_DIR}/mycalc/external/imgui/imgui_widgets.cpp"
    "${PROJECT_SOURCE_DIR}/mycalc/external/imgui/imgui_demo.cpp"

    "${PROJECT_SOURCE_DIR}/mycalc/external/imgui/backends/imgui_impl_glfw.cpp"
    "${PROJECT_SOURCE_DIR}/mycalc/external/imgui/backends/imgui_impl_opengl3.cpp"
)

target_include_directories(mycalc PRIVATE
    "${PROJECT_SOURCE_DIR}/mycalc/external/imgui"
    "${PROJECT_SOURCE_DIR}/mycalc/external/imgui/backends"
)

target_link_libraries(mycalc PRIVATE
    gui
    glfw
    OpenGL::GL
)