#include <rclcpp/rclcpp.hpp>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdint>
#include <iostream>
#include "physicalLayer.h"
#include "dataProcessing.h"
#include "dataExecution.h"
#include "DSP.h"

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    // Test file for controlling the robot with UDP packet commands. 
    PhysicalLayer pL;
    DataProcessing dP;
    DataExecution dE;
    int blocksize = 379; 
    int samplerate = 8000; 
    int channels = 1; 
    int UdpID = 0; 
    int TcpID = 1; 
    std::vector<int> toner = {};
    double toneDuration = 0.07; // Duration of each tone in seconds

    DSP dsp(blocksize, samplerate, channels, {UdpID, TcpID});

    for(int i = 0; i < 20; i++) {
        toner = dsp.listen(samplerate, toneDuration);
        for (int t : toner) {
            std::cout << t << " ";
        }
        std::cout << std::endl;

        pL.queueData(toner);
        const std::vector<std::int32_t> packet = pL.getData();

        std::cout << "Received packet: {";
        for (std::size_t i = 0; i < packet.size(); i++) {
            std::cout << packet[i] << (i + 1 < packet.size() ? ", " : "");
        }
        std::cout << "}\n";

        const int robotID = packet[0];
        dP.setData(packet);

        if (!dP.isValidPacket(robotID, packet)) {
            std::cout << "INVALID\n"; 
        }

        if (dP.typeControl(packet).value()) {
            const bool queued = dP.queueTcpPacket(robotID);
            std::cout << "valid TCP, " << (queued ? "queued" : "not queued (duplicate)") << "\n";
        } else {
            float zVel = dP.getZVelocity(robotID);
            float xVel = dP.getXVelocity();
            std::cout << "valid UDP, angle " << zVel
                        << ", xVelocity " << xVel << "\n";          
            dE.moveRobot(0.1, zVel);
            // dE.stopRobot();
        }
        
    }
    dE.stopRobot();
    rclcpp::shutdown();

    
}
