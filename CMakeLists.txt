cmake_minimum_required(VERSION 3.5)
project(test)

# Find librealsense
find_package(realsense2 REQUIRED)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)

# Add your source files
set(SOURCES
    main.cpp
    # Add your other source files here
)

# Include directories
include_directories(
${CMAKE_SOURCE_DIR}/include
${CMAKE_SOURCE_DIR}/imgui
)

# Create the executable
add_executable(test ${SOURCES})

# Link against librealsense
target_link_libraries(test PRIVATE ${realsense2_LIBRARY})

# Add ImGui sources to your project
add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    # Add other ImGui source files if needed
)

# Link against ImGui
target_link_libraries(test PRIVATE imgui)
