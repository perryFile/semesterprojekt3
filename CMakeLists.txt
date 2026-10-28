cmake_minimum_required(VERSION 3.10)
project(turtlebot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)

add_executable(turtlebot_node
    test/dataExecution.cpp
    test/dataProcessing.cpp
    test/test_main.cpp
)

target_include_directories(turtlebot_node PRIVATE test)
ament_target_dependencies(turtlebot_node rclcpp geometry_msgs)
