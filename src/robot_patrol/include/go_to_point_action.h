#ifndef GO_TO_POINT_HPP_
#define GO_TO_POINT_HPP_

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "geometry_msgs/msg/point32.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2/convert.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.h"
#include "robot_patrol/action/go_to_point.hpp"


using GoToPointAction = robot_patrol::action::GoToPoint;
using GoalHandleGoToPoint = rclcpp_action::ServerGoalHandle<GoToPointAction>;
using std::placeholders::_1;
using std::placeholders::_2;

class GoToPoint : public rclcpp::Node
{
public:
    explicit GoToPoint(const rclcpp::NodeOptions &options = rclcpp::NodeOptions());
    ~GoToPoint();
    
private:
    void odomCallback(const nav_msgs::msg::Odometry::SharedPtr);
    void executeCB(const std::shared_ptr<GoalHandleGoToPoint>);

    rclcpp_action::GoalResponse handle_goal(const rclcpp_action::GoalUUID &uuid, std::shared_ptr<const GoToPointAction::Goal>);
    rclcpp_action::CancelResponse handle_cancel(const std::shared_ptr<GoalHandleGoToPoint>);
    void handle_accepted(const std::shared_ptr<GoalHandleGoToPoint>);

    rclcpp_action::Server<GoToPointAction>::SharedPtr action_server_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
    rclcpp::CallbackGroup::SharedPtr callback_group_;
    
    geometry_msgs::msg::Point32 desired_pos_;
    geometry_msgs::msg::Point32 current_pos_;
};

#endif