#include <rclcpp/rclcpp.hpp>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdint>
#include <iostream>
#include "physicalLayer.h"
#include "dataProcessing.h"
#include "dataExecution.h"



int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    // Test file for controlling the robot with TCP packet commands. 
    PhysicalLayer pL;
    DataProcessing dP;
    DataExecution dE;
    

    // Move the robot with the received TCP packet commands.
    while (rclcpp::ok()) {
        const std::vector<std::int32_t> packet = pL.getData();
        if (packet.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (!dP.typeControl(packet).value()) {
            // Received a UDP packet and move the robot
            dE.moveRobot(dP.getXVelocity(), dP.getZVelocity(packet[0]));
            std::cout << "Received UDP packet: {";
            for (std::size_t i = 0; i < packet.size(); i++) {
                std::cout << packet[i] << (i + 1 < packet.size() ?", " : "");
            }
            std::cout << "}, angle: " << dP.getAngle(packet[0])
                      << ", zVelocity: " << dP.getZVelocity(packet[0]) << "\n";
        }
        else {
            // Received a TCP packet and read the command
            dP.setData(packet);
            const int robotID = packet[0];
            if (dP.queueTcpPacket(robotID)) {
                const auto tcpPacket = dP.popTcpPacket();
                if (tcpPacket) {
                    dE.readCommand(*tcpPacket);
                    std::cout << "Received TCP packet: {";
                    for (std::size_t i = 0; i < packet.size(); i++) {
                        std::cout << packet[i] << (i + 1 < packet.size() ?", " : "");
                    }
                    std::cout << "}, command: " << tcpPacket->command << "\n";
                }
            }
        }
    }
    
    return 0;
}