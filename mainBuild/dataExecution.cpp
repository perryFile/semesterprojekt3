#include "dataExecution.h"

DataExecution::DataExecution()
    : node_(rclcpp::Node::make_shared("data_execution")),
      pub_(node_->create_publisher<geometry_msgs::msg::TwistStamped>("/cmd_vel", 10)) {}

void DataExecution::moveRobot(float xVelocity, float zVelocity) {
    geometry_msgs::msg::TwistStamped msg;
    msg.header.stamp = node_->now();
    msg.header.frame_id = "base_link";
    msg.twist.linear.x = xVelocity;
    msg.twist.angular.z = zVelocity;
    pub_->publish(msg);
}

void DataExecution::stopRobot() {
    moveRobot(0.0f, 0.0f);
}

void DataExecution::handleCommand(const ExecMsg& msg) {
    const auto now = std::chrono::steady_clock::now();

    switch (msg.commandType) {
        case ExecMsg::commandType::None:
            if (now < disabledUntil) {
                moveRobot(0.0f, 0.0f);          // Bomb aktiv
            } else {
                const float xVel = (inverted ? -msg.xVelocity : msg.xVelocity) * speedScale;
                moveRobot(xVel, msg.zVelocity);
            }
            break;
        case ExecMsg::commandType::Bomb:
            disabledUntil = now + std::chrono::seconds(3);
            moveRobot(0.0f, 0.0f);
            break;
        case ExecMsg::commandType::SpeedBoost:
            speedScale = 0.22f / 0.15f;         // 0.15 -> 0.22
            break;
        case ExecMsg::commandType::InvertedControls:
            inverted = !inverted;
            break;
        case ExecMsg::commandType::StopAbility:
            speedScale = 1.0f;
            inverted = false;
            break;
    }
}