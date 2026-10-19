//dataExecution.cpp
#include "dataExecution.h"

DataExecution::DataExecution() : node_(rclcpp::Node::make_shared("data_execution")), pub_(node_->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10)) {
    // Constructor: sets up the ROS publisher
}

void DataExecution::executeCommand(int commandIndex) {

    //Error handling - kan slettes hvis kode oprydes
    if (commandIndex < 0 || commandIndex >= static_cast<int>(COMMANDS.size())) {
        RCLCPP_WARN(node_->get_logger(), "Invalid command index: %d", commandIndex);
        return;
    }

    const MoveCommand& cmd = COMMANDS[commandIndex];
    geometry_msgs::msg::Twist twist_msg;
    twist_msg.linear.x = cmd.x;
    twist_msg.angular.z = cmd.z;

    pub_->publish(twist_msg);
}

DataExecution::~DataExecution() {}