cmake_minimum_required(VERSION 3.16)

project(mainBuild)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ROS 2 dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)

# Opret executable
add_executable(main
    main.cpp
    init.cpp
    DSP.cpp
    dataExecution.cpp
    dataProcessing.cpp
    physicalLayer.cpp
)
add_executable(mainPlayer
    mainPlayer.cpp
    DSP.cpp
    init.cpp
)

# PortAudio stier
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/include
)

target_link_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/lib/.libs
)

# Link PortAudio
target_link_libraries(main portaudio)

target_include_directories(mainPlayer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/include
)

target_link_directories(mainPlayer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/lib/.libs
)

target_link_libraries(mainPlayer portaudio)

# ROS 2 dependencies
ament_target_dependencies(main
    rclcpp
    geometry_msgs
)