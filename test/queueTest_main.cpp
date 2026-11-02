#include <cstdint>
#include <iostream>
#include <vector>
#include "dataProcessing.h"

int main() {
    const std::vector<std::vector<std::int32_t>> packets = {
        {1, 0, 4, 2, 4, 12},
        {1, 0, 1, 11, 3, 2},
        {2, 0, 9, 0},
        {0, 1, 6, 8},
        {1, 0, 4, 2, 4, 13},
    };

    DataProcessing dp;

    for (const auto& packet : packets) {
        std::cout << "{";
        for (std::size_t i = 0; i < packet.size(); i++) {
            std::cout << packet[i] << (i + 1 < packet.size() ? ", " : "");
        }
        std::cout << "}  ->  ";

        if (packet.empty()) {
            std::cout << "empty\n";
            continue;
        }

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
            std::cout << "valid UDP, angle " << dp.getAngle(robotID)
                      << ", zVelocity " << dp.getZVelocity(robotID) << "\n";
        }
    }

    std::cout << "\nTCP queue (" << dp.tcpQueueSize() << " packets):\n";
    while (auto p = dp.popTcpPacket()) {
        std::cout << "  robot " << p->robotID
                  << ", packet nr " << p->packetNumber
                  << ", command " << p->command << "\n";
    }

    return 0;
}