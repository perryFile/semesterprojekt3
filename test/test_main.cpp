// test_main.cpp
#include <rclcpp/rclcpp.hpp>
#include <thread>
#include <chrono>
#include <vector>
#include <cstdint>
#include <iostream>
#include "dataProcessing.h"
#include "dataExecution.h"

// Prints PASS/FAIL for one check and counts failures
static int failures = 0;

static void check(const std::string& name, bool actual, bool expected) {
    const bool ok = (actual == expected);
    std::cout << (ok ? "[PASS] " : "[FAIL] ") << name
              << "  (got " << actual << ", expected " << expected << ")\n";
    if (!ok) failures++;
}

static void runCrcTests() {
    DataProcessing dp;

    // ---- Valid TCP packets ----
    // Payload {1,0,4,2} -> CRC 0x4C = 76 -> tones 4 and 12 (C)
    const std::vector<std::int32_t> good1 = {1, 0, 4, 2, 4, 12};
    check("valid TCP packet {1,0,4,2,4,12}", dp.isValidPacket(1, good1), true);
    check("crcCheck on {1,0,4,2,4,12}",      dp.crcCheck(good1),         true);

    // Payload {1,0,1,11} -> CRC 0x32 = 50 -> tones 3 and 2
    const std::vector<std::int32_t> good2 = {1, 0, 1, 11, 3, 2};
    check("valid TCP packet {1,0,1,11,3,2}", dp.isValidPacket(1, good2), true);

    // ---- Corrupt each tone, one at a time (every one must be rejected) ----
    for (std::size_t i = 0; i < good1.size(); i++) {
        std::vector<std::int32_t> bad = good1;
        bad[i] = (bad[i] + 1) % 16;            // still a valid tone value, but wrong
        check("corrupted tone at index " + std::to_string(i),
              dp.crcCheck(bad), false);
    }

    // ---- Invalid tone values ----
    check("tone value 16 is rejected", dp.crcCheck({1, 0, 4, 2, 4, 16}), false);
    check("tone value -1 is rejected", dp.crcCheck({1, 0, 4, 2, 4, -1}), false);

    // ---- Packet type / length checks in isValidPacket ----
    // ID 0 is UDP in the table, so a 6-tone (TCP) packet must be rejected
    check("UDP robot with TCP-sized packet", dp.isValidPacket(0, good1), false);

    // ID 1 is TCP in the table, so a 4-tone (UDP) packet must be rejected
    const std::vector<std::int32_t> udpPacket = {0, 0, 1, 11};
    check("TCP robot with UDP-sized packet", dp.isValidPacket(1, udpPacket), false);

    // UDP packets have no CRC and pass on type alone
    check("valid UDP packet {0,0,1,11}", dp.isValidPacket(0, udpPacket), true);

    // Wrong length (5 tones)
    check("invalid length (5 tones)", dp.isValidPacket(1, {1, 0, 4, 2, 4}), false);

    // Unknown robot ID
    check("unknown robot ID 9", dp.isValidPacket(9, good1), false);
}

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    // ---- CRC tests ----
    runCrcTests();
    if (failures > 0) {
        std::cout << "\n" << failures << " test(s) FAILED, not moving the robot.\n";
        rclcpp::shutdown();
        return 1;
    }
    std::cout << "\nAll CRC tests passed.\n\n";

    // ---- Robot test ----
    const int RobotID = 0;
    std::vector<std::int32_t> data = {1, 0, 1, 11};

    DataProcessing dataP(data);
    float xVel = 0.1f;
    float zVel = dataP.getZVelocity(RobotID);

    DataExecution dataE(xVel, zVel);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    dataE.moveRobot(xVel, zVel);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    dataE.moveRobot(0.0f, 0.0f);   // stop the robot
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    rclcpp::shutdown();
    return 0;
}