// test_main.cpp
#include <iostream>
#include <vector>
#include <cstdint>
#include "dataProcessing.h"

int main() {
    const int uRobotID = 1;
    const int tRobotID = 2;
    const int uRobotID2 = 3;
    const int tRobotID2 = 4;

    // robotID 1, angle 90 -> 0x05A -> dele 0, 5, 10
    std::vector<std::int32_t> data = {1, 0, 5, 10};

    DataProcessing dataProcessor(data);

    std::cout << "Angle for robot " << uRobotID << ": "
              << dataProcessor.getAngle(uRobotID) << "\n";   // expected 90
    std::cout << "Angle for robot " << uRobotID2 << ": "
              << dataProcessor.getAngle(uRobotID2) << "\n";   // expected 0 (wrong ID)
    std::cout << "z velocity: " << dataProcessor.getZVelocity(1) << "\n";


    return 0;
}