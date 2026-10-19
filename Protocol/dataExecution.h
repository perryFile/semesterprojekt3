// DataExecution.h
#ifndef DATAEXECUTION_H
#define DATAEXECUTION_H

#include <memory>
#include <vector>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>


struct MoveCommand {
    double x;   // linear velocity along the robot's forward x-axis
    double z;   // angular velocity around the vertical z-axis (positive = left)
};

class DataExecution {
public:
    DataExecution();                        
    void executeCommand(int commandIndex); 
    ~DataExecution() = default;

private:
    // Index in the vector = command number received in the data field
    const std::vector<MoveCommand> COMMANDS = {
        { 0.00,  0.0 },   // 0: stop
        { 0.10,  0.0 },   // 1: forward
        {-0.10,  0.0 },   // 2: backward
        { 0.05,  0.2 },   // 3: left (drive forward while turning left)
        { 0.05, -0.2 },   // 4: right (drive forward while turning right)
        { 0.00,  0.2 },   // 5: rotate left (turn in place)
        { 0.00, -0.2 },   // 6: rotate right (turn in place)
        { 0.00,  0.0 }    // 7: stop
    };

    std::shared_ptr<rclcpp::Node> node_; //ROS node 
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_; // Publisher for sending movement commands
};

#endif // DATAEXECUTION_H