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
        bool checkId(int robotID, const std::vector<std::int32_t>& data) const;
        float getAngle(int robotID) const;
        float getXVelocity() const;
        float getZVelocity(int robotID) const;
        float convertAngleData(const std::vector<std::int32_t>& rawAngle) const;
        std::optional<bool> typeControl(const std::vector<std::int32_t>& inputData) const;
        bool isValidPacket(int robotID, const std::vector<std::int32_t>& inputData) const;
        bool crcCheck(const std::vector<std::int32_t>& inputData) const;
        bool queueTcpPacket(int robotID);
        std::optional<TcpPacket> popTcpPacket();
        std::size_t tcpQueueSize() const;
        std::int32_t receivedTcpPacket();

        ~DataProcessing() = default;

    private:
        static std::uint8_t crc8(const std::vector<std::int32_t>& data, std::size_t count);

        //skal 100% omnavngives
        const std::vector<RobotPacketType> robotIDtype = {
            {1, false},  // id 1: robot 1, UDP
            {2, true},   // id 2: robot 1, TCP
            {3, false},  // id 3: robot 2, UDP
            {4, true}    // id 4: robot 2, TCP
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
        std::queue<TcpPacket> tcpQueue;
        std::map<int, int> lastPacketNumber;
    };

#endif // DATAPROCESSING_H