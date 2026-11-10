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

std::mutex mtx;
std::vector<int> toner = {};
std::vector<std::int32_t> packet;
std::vector<std::int32_t> ackPacket;
int robotID;
bool stop = false;

// Test file for controlling the robot with UDP packet commands. 
int blocksize = 379; 
int samplerate = 8000; 
int channels = 1; 
int UdpID = 0; 
int TcpID = 1; 
PhysicalLayer pL(blocksize, samplerate, channels, {UdpID, TcpID});
DataProcessing dP;
DataExecution dE;


void listenForDTMF(){
    double toneDuration = 0.07; // Duration of each tone in seconds
    while(true){
        mtx.lock();
        toner = pL.listen(samplerate, toneDuration);

        //Skal fjernes cout senere
        for (int t : toner) {
            std::cout << t << " ";
        }
        std::cout << std::endl;
        pL.queueData(toner);

        packet = pL.getData();

        //Skal fjernes cout senere
        std::cout << "Received packet: {";
        for (std::size_t i = 0; i < packet.size(); i++) {
            std::cout << packet[i] << (i + 1 < packet.size() ? ", " : "");
        }
        std::cout << "}\n";
        mtx.unlock();
    }
}

void processAndExecute(){
    //Vi skal have lavet en måde hvorpå vi kan se om vi har processeret denne pakke. evt queue?
    while(true){
        mtx.lock();
        robotID = packet[0];
        dP.setData(packet);

        //Skal fjernes senere
        if (!dP.isValidPacket(robotID, packet)) {
            std::cout << "INVALID\n"; 
        }

        if (dP.typeControl(packet).value()) {
            const bool queued = dP.queueTcpPacket(robotID);
            
            //Sende ACK tilbage. 
            //***DISSE VÆRDIER SKAL MODIFICERES***
            int robotID_x = 2;
            int packetNumber = 2;
            int computerID = 4;

            if (queued){
                ackPacket = dP.ACKTcpPacket(computerID, robotID_x, packetNumber); // Vi modtager en TCP pakke, så vi sender en ACK tilbage til robotten
                
                // ***Tilføj thread til at ACKE.***
                std::thread(ackTCP).detach(); //Thread starter ny thread som afsluttes efter.

                std::cout << "ACK packet: {";
                for (std::size_t i = 0; i < ackPacket.size(); i++) {
                    std::cout << ackPacket[i] << (i + 1 < ackPacket.size() ? ", " : "");
                }
                std::cout << "}\n";
                pL.sendData(ackPacket); // Send ACK packet back to robot
            }
            std::cout << "valid TCP, " << (queued ? "queued" : "not queued (duplicate)") << "\n";
            
            
        } else {    
            float zVel = dP.getZVelocity(robotID);
            float xVel = dP.getXVelocity();
            std::cout << "valid UDP, angle " << zVel
                        << ", xVelocity " << xVel << "\n";          
            dE.moveRobot(xVel, zVel);
            
        }
        mtx.unlock(); //Måske skal denne sættes et andet sted?    
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

    //Queues for data
    Queue pLTodP;
    Queue dPTopL;

    std::thread t1(listenForDTMF);
    std::thread t2(processAndExecute);

    while(stop == false){
        
    }
    //Lige så snart stop bliver true hopper 
    //den ud af while loopet og slutter de to threads sammen
    //og stopper robotten.

    t1.join(); 
    t2.join();
    dE.stopRobot();
    rclcpp::shutdown();

    return 0;
}
