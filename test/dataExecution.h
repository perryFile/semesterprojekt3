// DataExecution.h
#ifndef DATAEXECUTION_H
#define DATAEXECUTION_H

#include <memory>
#include <vector>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include "dataProcessing.h"


class DataExecution {
public:
    DataExecution(float xVelocity = 0.0f, float zVelocity = 0.0f);
    void moveRobot(float xVelocity, float zVelocity);
    ~DataExecution() = default;

private:
    float xVelocity;
    float zVelocity;
    std::shared_ptr<rclcpp::Node> node_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr pub_;
};

#endif // DATAEXECUTION_H