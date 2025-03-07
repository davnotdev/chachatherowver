cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(rowverone)

file(GLOB_RECURSE dataview_src)

add_subdirectory(src/dataview/glfw)
add_subdirectory(src/dataview/glm)

add_executable(
    dataview 

    src/dataview/main.cpp
    src/dataview/mainui.cpp
    src/dataview/serialin.cpp
    src/dataview/ui.cpp
    src/dataview/blob.cpp
    src/dataview/filter.cpp

    src/dataview/imgui/imgui.cpp
    src/dataview/imgui/imgui_demo.cpp
    src/dataview/imgui/imgui_draw.cpp
    src/dataview/imgui/imgui_widgets.cpp
    src/dataview/imgui/imgui_tables.cpp
    src/dataview/imgui/backends/imgui_impl_glfw.cpp
    src/dataview/imgui/backends/imgui_impl_opengl3.cpp

    src/dataview/glad/src/glad.c
)

target_include_directories(
    dataview PRIVATE 

    src/dataview/
    src/dataview/glad/include/
    src/dataview/glfw/include/
    src/dataview/glm/
    src/dataview/imgui/
    src/dataview/imgui/backends/
)
target_link_libraries(dataview glfw glm)

