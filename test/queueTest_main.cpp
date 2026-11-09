#include <cstdint>
#include <iostream>
#include <vector>
#include "dataProcessing.h"

int main() {
    PhysicalLayer physicalLayer;
    DataProcessing dp;

    physicalLayer.queueData({1, 0, 4, 2, 4, 12});
    physicalLayer.queueData({1, 0, 4, 2, 4, 12}); // duplicate packet number
    physicalLayer.queueData({1, 0, 1, 11, 3, 2});
    physicalLayer.queueData({0, 0, 1, 11});

    while (true) {
        const std::vector<std::int32_t> packet = physicalLayer.getData();
        if (packet.empty()) {
            break;
        }

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
            std::cout << "valid UDP, angle " << dp.getAngle(robotID)
                      << ", zVelocity " << dp.getZVelocity(robotID) << "\n";
        }
    }

    std::cout << "\nTCP queue (" << dp.tcpQueueSize() << " packets):\n";
    while (const auto queuedPacket = dp.popTcpPacket()) {
        std::cout << "  robot " << queuedPacket->robotID
                  << ", packet nr " << queuedPacket->packetNumber
                  << ", command " << queuedPacket->command << "\n";
    }

    return 0;
}