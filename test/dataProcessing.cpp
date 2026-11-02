// dataProcessing.cpp
#include "dataProcessing.h"

DataProcessing::DataProcessing() {}

DataProcessing::DataProcessing(const std::vector<std::int32_t>& inputData)
    : data(inputData) {}


bool DataProcessing::checkId(int robotID, const std::vector<std::int32_t>& data) const {
    return !data.empty() && data[0] == robotID;
}

float DataProcessing::getAngle(int robotID) const {
    if (!checkId(robotID, data)) {
        return 0.0f;
    }

    // data[0] is robotID, everything after it is the angle
    if (data.size() < 2) {
        return 0.0f;
    }

    std::vector<std::int32_t> rawAngle(data.begin() + 1, data.end());

    return convertAngleData(rawAngle);
}

float DataProcessing::getXVelocity() const {
    return xVelocity;
}

float DataProcessing::getZVelocity(int robotID) const {
    const float angle = getAngle(robotID);

    // Vi gør så værdien ikke overstiger 0.22
    const float clamped = std::clamp(angle, 0.0f, maxAngle);

    return clamped / maxAngle * maxZVelocity;
}

float DataProcessing::convertAngleData(const std::vector<std::int32_t>& rawAngle) const {
    std::string hexAngle = "";

    for (std::size_t i = 0; i < rawAngle.size(); i++) {
        if (rawAngle[i] == 10) {
            hexAngle += "A";
        }
        else if (rawAngle[i] == 11) {
            hexAngle += "B";
        }
        else if (rawAngle[i] == 12) {
            hexAngle += "C";
        }
        else if (rawAngle[i] == 13) {
            hexAngle += "D";
        }
        else if (rawAngle[i] == 14) {
            hexAngle += "E";
        }
        else if (rawAngle[i] == 15) {
            hexAngle += "F";
        }
        else if (rawAngle[i] >= 0 && rawAngle[i] <= 9) {
            hexAngle += std::to_string(rawAngle[i]);
        }
        else {
            return 0.0f;   
        }
    }

    if (hexAngle.empty()) {
        return 0.0f;
    }

    // "3E8" -> 1000
    return static_cast<float>(std::stoi(hexAngle, nullptr, 16));
}

std::optional<bool> DataProcessing::typeControl(const std::vector<std::int32_t>& inputData) const {
    if (inputData.size() == UDP_PacketSize) return false;   // UDP
    if (inputData.size() == TCP_PacketSize) return true;    // TCP
    return std::nullopt;                                    // ugyldig længde
}
//Passer robotID med om pakken der er sendt er en TCP eller UDP
bool DataProcessing::isValidPacket(int robotID, const std::vector<std::int32_t>& inputData){
    //False = UDP, true = TCP, tom = ugyldig længde
    const auto type = typeControl(inputData); 
    
    for (std::size_t i = 0; i < robotIDtype.size(); i++) {
        if (robotIDtype[i].robotNr == robotID) {          
            const bool robotType = robotIDtype[i].type; 

            if (robotType == *type) {
                
                return true;    
            }
            return false;       
        }
    }

    return false;              
    
}