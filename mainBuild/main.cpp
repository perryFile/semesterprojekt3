#include <rclcpp/rclcpp.hpp>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdint>
#include <iostream>
#include "physicalLayer.h"
#include "dataProcessing.h"
#include "dataExecution.h"
#include "controller.h"
#include "DSP.h"

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    // Test file for controlling the robot with UDP packet commands. 
    PhysicalLayer pL;
    DataProcessing dP;
    DataExecution dE;
    int blocksize = 379; 
    int samplerate = 8000; 
    int numIdBits = 1; 
    int UdpID = 1; 
    int TcpID = 2; 
    std::vector<int> toner = {};
    double toneDuration = 0.07; // Duration of each tone in seconds

    DSP dsp(blocksize, samplerate, numIdBits, {UdpID, TcpID});

    toner = dsp.listen(samplerate, toneDuration);
    for (int t : toner) {
        std::cout << t << " ";
    }
    std::cout << std::endl;

    pL.queue(toner);
    const std::vector<std::int32_t> packet = pL.getData();

    std::cout << "Received packet: {";
    for (std::size_t i = 0; i < packet.size(); i++) {
        std::cout << packet[i] << (i + 1 < packet.size() ? ", " : "");
    }
    std::cout << "}\n";

    const int robotID = packet[0];
    dp.setData(packet);

    if (!dp.isValidPacket(robotID, packet)) {
        std::cout << "INVALID\n";
        continue;
    }

    if (dp.typeControl(packet).value()) {
        const bool queued = dp.queueTcpPacket(robotID);
        std::cout << "valid TCP, " << (queued ? "queued" : "not queued (duplicate)") << "\n";
    } else {
        float zVel = dp.getZVelocity(robotID);
        float xVel = dp.getXvelocity();
        std::cout << "valid UDP, angle " << dp.getZVelocity()
                    << ", xVelocity " << dp.getXVelocity() << "\n";
        dE.moveRobot(xVel, zVel);
    }


    

    
}
