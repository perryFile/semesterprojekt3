
#include "dataExecution.h"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

DataExecution::DataExecution(float xVelocity, float zVelocity)
    : xVelocity(xVelocity),
      zVelocity(zVelocity),
      node_(rclcpp::Node::make_shared("data_execution")),
      pub_(node_->create_publisher<geometry_msgs::msg::TwistStamped>("/cmd_vel", 10)) {}


void DataExecution::moveRobot(float xVelocity, float zVelocity) {
    geometry_msgs::msg::TwistStamped msg;
    msg.header.stamp = node_->now();
    msg.header.frame_id = "base_link";
    msg.twist.linear.x = xVelocity;
    msg.twist.angular.z = zVelocity;
    pub_->publish(msg);
}

void DataExecution::readCommand(struct TcpPacket packet) {
    // Assuming command is a simple integer that maps to specific actions
    switch (packet.command) {
        case 0: // Bomb
            // Disable controls for a short duration
            moveRobot(0.0f, 0.0f);
            std::this_thread::sleep_for(3s); // Robot is disabled for 3 seconds
            break;
        case 1: // Speed boost
            xVelocity = 0.22f; // Set to maximum speed
            moveRobot(xVelocity, 0.0f);
            break;
        case 2: // Inverted controls
            // Invert the x-velocity for a short duration
            moveRobot(-xVelocity, 0.0f);
            break;
        case 3: // Stop ability
            xVelocity = 0.15f;
            break;
        default:
            RCLCPP_WARN(node_->get_logger(), "Unknown command received: %d", packet.command);
            break;
    }
}
