// DataProcessing.h
#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <cstdint>
#include <algorithm>
#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include "physicalLayer.h"

struct RobotPacketType {
    int robotNr; //Hvilket robot vi skal bruge
    bool type;   // 0/False = UDP, 1/True = TCP
};

class DataProcessing {
public:
    DataProcessing();
    explicit DataProcessing(const std::vector<std::int32_t>& inputData);
    
    bool checkId(int robotID, const std::vector<std::int32_t>& data) const;
    float getAngle(int robotID) const;
    float getXVelocity() const;
    float getZVelocity(int robotID) const;
    float convertAngleData(const std::vector<std::int32_t>& rawAngle) const;
    std::optional<bool> typeControl(const std::vector<std::int32_t>& inputData) const; 
    bool isValidPacket(int robotID, const std::vector<std::int32_t>& inputData);
    ~DataProcessing() = default;

private:
    //skal 100% omnavngives
    const std::vector<RobotPacketType> robotIDtype = {
        {0, false},  // id 0: robot 1, UDP
        {1, true},   // id 1: robot 1, TCP
        {2, false},  // id 2: robot 2, UDP
        {3, true}    // id 3: robot 2, TCP
    };

    static constexpr std::size_t UDP_PacketSize = 4;
    static constexpr std::size_t TCP_PacketSize = 6;
    std::vector<std::int32_t> data;
    std::vector<std::int32_t> command;
    std::vector<std::int32_t> rawAngle;
    float angle = 0.00f;
    float xVelocity = 0.22f;
    static constexpr float maxAngle     = 360.0f;  
    static constexpr float maxZVelocity = 0.22f; 
};

#endif // DATAPROCESSING_H