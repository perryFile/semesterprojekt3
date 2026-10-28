// test_main.cpp
#include <rclcpp/rclcpp.hpp>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdint>
#include "dataProcessing.h"
#include "dataExecution.h"


int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    const int uRobotID = 1;
    std::vector<std::int32_t> data = {1, 0, 1, 11};

    DataProcessing dataP(data);
    float xVel = 0.1f;
    float zVel = dataP.getZVelocity(uRobotID);

    
    DataExecution dataE(xVel, zVel);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    dataE.moveRobot(xVel, zVel);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    dataE.moveRobot(0.0f, 0.0f);   // stop the robot
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    

    rclcpp::shutdown();
    return 0;
}