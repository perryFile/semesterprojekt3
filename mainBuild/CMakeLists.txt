cmake_minimum_required(VERSION 3.16)

project(DSP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)

# Opret executable
add_executable(maintest
    main.cpp
    init.cpp
    DSP.cpp
    controller.cpp
    dataExcecution.cpp
    dataProcessing.cpp
    physicalLayer.cpp

)

# PortAudio stier
target_include_directories(maintest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/include
)

target_link_directories(maintest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/lib/.libs
)

# Link PortAudio
target_link_libraries(maintest PRIVATE portaudio)

target_include_directories(turtlebot_node PRIVATE test)
ament_target_dependencies(turtlebot_node rclcpp geometry_msgs)