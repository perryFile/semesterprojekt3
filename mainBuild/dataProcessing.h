// DataProcessing.h
#ifndef DATAPROCESSING_H
#define DATAPROCESSING_H

#include <array>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <queue>
#include <string>
#include <vector>
#include "queue.h"

struct RobotPacketType {
    int robotNr; // Hvilket robot vi skal bruge
    bool type;   // 0/False = UDP, 1/True = TCP
};

struct TcpPacket {
    int robotID;
    int packetNumber;
    int command;
};

class DataProcessing {
    public:
        DataProcessing();
        explicit DataProcessing(const std::vector<std::int32_t>& inputData);
        void setData(const std::vector<std::int32_t>& inputData);
        
        // Movement and velocity functions
        float getAngle(int robotID) const;
        float getXVelocity() const;
        float getZVelocity(int robotID) const;
        float convertAngleData(const std::vector<std::int32_t>& rawAngle) const;
        
        // Error checking functions
        bool checkId(int robotID, const std::vector<std::int32_t>& data) const;
        std::optional<bool> typeControl(const std::vector<std::int32_t>& inputData) const;
        bool isValidPacket(int robotID, const std::vector<std::int32_t>& inputData) const;
        bool crcCheck(const std::vector<std::int32_t>& inputData) const;
        
        std::optional<TcpPacket> decodeTcpPacket(int robotID, bool& isDuplicate);
        std::vector<std::int32_t> ACKTcpPacket(std::int32_t computerID, std::int32_t robotID, std::int32_t packetNumber);
        int getCommand(const TcpPacket& packet);

        ~DataProcessing() = default;

    private:
        static std::uint8_t crc8(const std::vector<std::int32_t>& data, std::size_t count);

        const std::vector<RobotPacketType> robotIDtype = {
            {0, false},  // id 0: robot 1, UDP
            {1, true},   // id 1: robot 1, TCP
            {2, false},  // id 2: robot 2, UDP
            {3, true}    // id 3: robot 2, TCP
        };

        static constexpr std::size_t UDP_PacketSize = 4;
        static constexpr std::size_t TCP_PacketSize = 6;
        std::vector<std::int32_t> data;
        float xVelocity = 0.15f; // Kan ikke køre hurtigere end 0.22f, da robotten ikke kan følge med.
        static constexpr float maxAngle     = 4095.0f;
        static constexpr float maxZVelocity = 1.0f;
        std::map<int, int> lastPacketNumber;

};

#endif // DATAPROCESSING_H