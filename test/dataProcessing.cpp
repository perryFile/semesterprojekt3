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
bool DataProcessing::isValidPacket(int robotID, const std::vector<std::int32_t>& inputData) {
    //False = UDP, true = TCP, tom = ugyldig længde
    const auto type = typeControl(inputData);
    if (!type) return false;

    for (std::size_t i = 0; i < robotIDtype.size(); i++) {
        if (robotIDtype[i].robotNr == robotID) {
            const bool robotType = robotIDtype[i].type;

            if (robotType != *type) {
                return false;   // forkert pakketype til dette ID
            }

            // Kun TCP-pakker har en CRC
            return *type ? crcCheck(inputData) : true;
        }
    }

    return false;
}

// CRC-8: poly 0x07, init 0x00, no reflection, no xorout.
// Hver tone er sin egen byte (0-15)
// Argumenter: 
// data: Data fra vores toner 
// count: Antallet af bytes som ikke er CRC8 værdier
std::uint8_t DataProcessing::crc8(const std::vector<std::int32_t>& data, std::size_t count) {
    const std::uint8_t POLYNOMIAL = 0x07;   // Divisoren i CRC8
    const std::uint8_t TOP_BIT    = 0x80;   // 10000000

    std::uint8_t crc = 0x00;                // start value

    // Vi går igennem værdierne en ad gangen
    for (std::size_t i = 0; i < count; i++) {

        // Tilføj den aktuelle tone til crc
        std::uint8_t tone = static_cast<std::uint8_t>(data[i]);
        crc ^= tone;

        // 2. Gå igennem hver bit i den aktuelle byte
        for (int bit = 0; bit < 8; bit++) {

            // Gem værdien af af leftmost bit inden vi shifter mod venstre 
            bool topBitIsSet = (crc & TOP_BIT) != 0;

            // Shift alting mod venstre så vi går videre til den næste bit
            crc = static_cast<std::uint8_t>(crc << 1);

            // Hvis vi mistede en bit så tilføj den til crc 
            if (topBitIsSet) {
                crc ^= POLYNOMIAL;
            }
        }
    }

    return crc;
}
// Last two tones = CRC (first one is the most significant digit).
// Everything before them is covered by the CRC.
bool DataProcessing::crcCheck(const std::vector<std::int32_t>& inputData) const {
    if (inputData.size() < 3) return false;

    for (std::int32_t t : inputData) {
        if (t < 0 || t > 15) return false;      // invalid tone
    }

    const std::size_t payloadLen = inputData.size() - 2;
    const int received = inputData[payloadLen] * 16 + inputData[payloadLen + 1];

    return crc8(inputData, payloadLen) == received;
}