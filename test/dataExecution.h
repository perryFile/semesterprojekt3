// DataExecution.h
#ifndef DATAEXECUTION_H
#define DATAEXECUTION_H

#include <memory>
#include <vector>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include "dataProcessing.h"

struct MoveCommand {
    double x;   // linear velocity along the robot's forward x-axis
    double z;   // angular velocity around the vertical z-axis (positive = left)
};

class DataExecution {
public:
    DataExecution();  
    DataExecution(int xVelocity, int zVelocity);
    void moveRobot(int xVelocity, int zVelocity);                     
    ~DataExecution() = default;

private:
  
    std::shared_ptr<rclcpp::Node> node_; //ROS node 
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr pub_; // Publisher for sending movement commands
};

#endif // DATAEXECUTION_H