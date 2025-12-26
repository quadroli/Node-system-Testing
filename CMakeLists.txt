# Minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Define your project name and language
project(GraphEditor VERSION 1.0 LANGUAGES CXX C)

# Standard C++ settings (e.g., C++17 standard)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify include directories for headers
include_directories(
    lib/imgui
    lib/imgui/backends
)

# Define a variable for the source files
set(SOURCES
    # Your main application file
    node.cpp 
    
    # ImGui source files (adjust paths to match your layout)
    lib/imgui-master/imgui.cpp
    lib/imgui-master/imgui_draw.cpp
    lib/imgui-master/imgui_widgets.cpp 
    lib/imgui-master/imgui_tables.cpp 
    lib/imgui-master/backends/imgui_impl_glfw.cpp
    lib/imgui-master/backends/imgui_impl_opengl3.cpp
    lib/glew-2.1.0/src/glew.c
)

add_subdirectory(lib/glfw-3.4)

# Create the executable target
add_executable(Bambuu ${SOURCES})

# Link against the necessary system libraries
# These are the equivalents of -lglfw -lopengl32 -lgdi32 on MinGW/Windows
target_link_libraries(Bambuu PUBLIC
    glfw
    GL
)

target_include_directories(Bambuu
    PUBLIC
        lib/imgui-master
        lib.glfw-3.4/include
        lib/glew-2.1.0/include
)

target_compile_options(Bambuu PUBLIC 
    $<$<CXX_COMPILER_ID:MSVC>:/W0>
)
