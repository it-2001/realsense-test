cmake_minimum_required(VERSION 3.5)
project(test)

# Find librealsense
find_package(realsense2 REQUIRED)

# Find OpenCV
find_package(OpenCV REQUIRED)

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
    /home/petr/opencv-4.x/3rdparty/include/opencl/1.2
)

# Create the executable
add_executable(test ${SOURCES})

# Link against librealsense and OpenCV
target_link_libraries(test PRIVATE 
    ${realsense2_LIBRARY}
    ${OpenCV_LIBRARIES}
)
