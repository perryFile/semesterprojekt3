
#include "dataExecution.h"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

DataExecution::DataExecution()
    : node_(rclcpp::Node::make_shared("data_execution")),
      pub_(node_->create_publisher<geometry_msgs::msg::TwistStamped>(
          "/cmd_vel", 10))
{
}

DataExecution::DataExecution(int xVelocity, int zVelocity)
    : xVelocity(xVelocity), zVelocity(zVelocity) {}

void DataExecution::executeCommand(std::vector<std::int32_t> commands)
{
    if (commands.empty()) {
        RCLCPP_WARN(node_->get_logger(), "Empty command vector");
        return;
    }

    for (size_t i = 0; i < commands.size(); ++i) {
        int commandIndex = commands[i];
        if (commandIndex < 0 ||
            commandIndex >= static_cast<int>(COMMANDS.size())) {
            RCLCPP_WARN(node_->get_logger(),
                        "Invalid command: %d", commandIndex);
            return;
        }
    }

    // Vent op til 3 sekunder på en subscriber
    for (int i = 0; i < 30 && rclcpp::ok(); ++i) {
        if (pub_->get_subscription_count() > 0) {
            break;
        }

        rclcpp::sleep_for(100ms);
    }

    if (pub_->get_subscription_count() == 0) {
        RCLCPP_WARN(node_->get_logger(),
                    "Ingen subscriber fundet på /cmd_vel");
    }

    for(size_t i = 0; i < commands.size(); ++i){
        const MoveCommand& cmd = COMMANDS[commands[i]];

        geometry_msgs::msg::TwistStamped msg;
        msg.header.stamp    = node_->now();
        msg.header.frame_id = "base_link";  
        msg.twist.linear.x  = cmd.x;
        msg.twist.angular.z = cmd.z;

        pub_->publish(msg);

        RCLCPP_INFO(node_->get_logger(),
                "Kommando %d sendt: linear.x=%.2f, angular.z=%.2f",
                commands[i], cmd.x, cmd.z);
    
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

