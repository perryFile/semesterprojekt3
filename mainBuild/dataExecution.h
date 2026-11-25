// DataExecution.h
#ifndef DATAEXECUTION_H
#define DATAEXECUTION_H

#include <memory>
#include <chrono>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>


class DataExecution {

public:
    DataExecution();
    void handleCommand(int robotID, int command);
    void stopRobot();
    void moveRobot(float xVelocity, float zVelocity);
    ~DataExecution() = default;

private:
    geometry_msgs::msg::TwistStamped msg;
    std::shared_ptr<rclcpp::Node> node_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr pub_;

    std::chrono::steady_clock::time_point disabledUntil{};
    float speedScale = 1.0f;
    float baseXVelocity = 0.15f;
    bool inverted = false;

};

#endif // DATAEXECUTION_H