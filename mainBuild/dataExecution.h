// DataExecution.h
#ifndef DATAEXECUTION_H
#define DATAEXECUTION_H

#include <memory>
#include <chrono>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>

struct ExecMsg {
    enum class commandType {
        None, 
        Bomb,
        SpeedBoost,
        InvertedControls,
        StopAbility
    };  
    float xVelocity = 0.0f;
    float zVelocity = 0.0f;
    commandType commandType = commandType::None;

};

class DataExecution {
public:
    DataExecution();
    void handleCommand(const ExecMsg& msg);
    void stopRobot();
    ~DataExecution() = default;

private:
    void moveRobot(float xVelocity, float zVelocity);
    
    std::shared_ptr<rclcpp::Node> node_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr pub_;

    std::chrono::steady_clock::time_point disabledUntil{};
    float speedScale = 1.0f;
    bool  inverted   = false;

};

#endif // DATAEXECUTION_H