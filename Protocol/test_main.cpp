#include <chrono>
#include <thread>
#include "DataExecution.h"

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    {
        DataExecution exec;

        // Vent lidt, så publisheren når at forbinde til robotten
        std::this_thread::sleep_for(std::chrono::seconds(1));

        exec.executeCommand(1);                                   // frem
        std::this_thread::sleep_for(std::chrono::seconds(2));

        exec.executeCommand(5);                                   // drej mod venstre
        std::this_thread::sleep_for(std::chrono::seconds(2));

        exec.stop();                                              // stop
    }
    rclcpp::shutdown();
}