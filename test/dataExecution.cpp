
#include "dataExecution.h"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

DataExecution::DataExecution()
    : node_(rclcpp::Node::make_shared("data_execution")),
      pub_(node_->create_publisher<geometry_msgs::msg::TwistStamped>("/cmd_vel", 10)){}

DataExecution::DataExecution(int xVelocity, int zVelocity)
    : xVelocity(xVelocity), zVelocity(zVelocity) {}


void DataExecution::moveRobot(int xVelocity, int zVelocity) {
    geometry_msgs::msg::TwistStamped msg;
    msg.twiststamped.linear.x = xVelocity;  // Set linear velocity
    msg.twiststamped.angular.z = zVelocity;  // Set angular velocity

    pub_->publish(msg);  // Publish the message to the /cmd_vel topic



}


DataExecution::~DataExecution() {
    rclcpp::shutdown();
}