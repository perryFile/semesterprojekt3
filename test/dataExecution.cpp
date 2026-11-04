
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

