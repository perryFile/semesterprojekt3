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
#include "queue.h"
#include <mutex>
#include <atomic>
#include <optional>

std::atomic<bool> stop = false;

// Test file for controlling the robot with UDP packet commands. 
int blocksize = 379; 
int samplerate = 8000; 
int channels = 1; 
int UdpID = 0; 
int TcpID = 1; 
const std::int32_t computerID = 4; // ID for the computer sending commands
std::vector<int> toner = {};

// Shared data structures
PhysicalLayer pL(blocksize, samplerate, channels, {UdpID, TcpID});
DataProcessing dP;
DataExecution dE;
Queue pLTodP; // Queue - PhysicalLayer -> DataProcessing
Queue dPTopL; // Queue - DataProcessing -> PhysicalLayer
Queue dPToDE; // Queue - DataProcessing -> DataExecution


void listenForDTMF() {
    const double toneDuration = 0.07;
    while (!stop) {
        std::vector<int> toner = pL.listen(samplerate, toneDuration);
        if (!toner.empty()) {
            pLTodP.push(std::vector<std::int32_t>(toner.begin(), toner.end()));
        }
    }
}

void sendThread() {
    while (!stop) {
        if (auto out = dPTopL.TryPop()) {
            std::this_thread::sleep_for(std::chrono::seconds(2));   // fjern hvis forsinkelsen ikke er nødvendig
            pL.sendData(*out);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}
void processAndExecute() {
    while (!stop) {
        auto maybePacket = pLTodP.TryPop();
        if (!maybePacket || maybePacket->empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        const std::vector<std::int32_t>& packet = *maybePacket;
        const int robotID = packet[0];

        if (robotID == computerID) continue;   // ignorer eget ekko

        dP.setData(packet);

        if (!dP.isValidPacket(robotID, packet)) {
            std::cout << "INVALID\n";
            continue;   // nu er typeControl(packet).value() sikker
        }

       if (dP.typeControl(packet).value()) {   // TCP
            bool isDuplicate = false;
            if (auto tcp = dP.decodeTcpPacket(robotID, isDuplicate)) {
                // ACK altid, også ved dublet
                dPTopL.push(dP.ACKTcpPacket(computerID, robotID, tcp->packetNumber));
                // Udfør kun hvis pakken er ny
                if (!isDuplicate) dE.readCommand(*tcp);
                std::cout << "valid TCP, " << (isDuplicate ? "dublet, kun ACK" : "udført") << "\n";
            }
        } else {                                 // UDP
            const float zVel = dP.getZVelocity(robotID);
            const float xVel = dP.getXVelocity();
            std::cout << "valid UDP, z " << zVel << ", x " << xVel << "\n";
            dE.moveRobot(xVel, zVel);
        }
    }
}

void ackTCP(){

    //**VI SKAL HAVE TILFØJET ACKPACKET TIL**
    //**EN QUEUE ELLER LAVE EN MUTEX ELLER NOGET PÅ DEN**
    std::this_thread::sleep_for(std::chrono::seconds(2));
    pL.sendData(ackPacket);
}

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    std::thread t1(listenForDTMF);
    std::thread t2(sendThread);
    std::thread t3(processAndExecute);

    while (rclcpp::ok() && !stop) std::this_thread::sleep_for(100ms);

    stop = true;
    pL.requestStop();   // Request to stop the DSP listening thread

    t1.join(); t2.join(); t3.join();
    dE.stopRobot();
    rclcpp::shutdown();

}
