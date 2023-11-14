#include "go_to_point_action.h"

GoToPoint::GoToPoint(const rclcpp::NodeOptions &options) : Node("go_to_point_node", options)
{
    callback_group_ = this->create_callback_group(rclcpp::CallbackGroupType::Reentrant);

    rclcpp::SubscriptionOptions options1;
    options1.callback_group = callback_group_;

    action_server_ = rclcpp_action::create_server<GoToPointAction>(
                     this,
                     "/go_to_point", 
                     std::bind(&GoToPoint::handle_goal, this, _1, _2),
                     std::bind(&GoToPoint::handle_cancel, this, _1),
                     std::bind(&GoToPoint::handle_accepted, this, _1));
    odom_sub_ = create_subscription<nav_msgs::msg::Odometry>("/odom", 1, std::bind(&GoToPoint::odomCallback, this, _1), options1);
    cmd_vel_pub_ = create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 1);

}

GoToPoint::~GoToPoint()
{}

void GoToPoint::odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg)
{
    current_pos_.x = msg->pose.pose.position.x;
    current_pos_.y = msg->pose.pose.position.y;
    tf2::Quaternion quaternion;
    tf2::fromMsg(msg->pose.pose.orientation, quaternion);
    double roll, pitch, yaw;
    tf2::Matrix3x3(quaternion).getRPY(roll, pitch, yaw);
    current_pos_.z = yaw;
}

rclcpp_action::GoalResponse GoToPoint::handle_goal(const rclcpp_action::GoalUUID & uuid, std::shared_ptr<const GoToPointAction::Goal> goal)
  {
    RCLCPP_INFO(this->get_logger(), "Received goal request with secs %d", goal->goal_pos);
    (void)uuid;
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  rclcpp_action::CancelResponse GoToPoint::handle_cancel(const std::shared_ptr<GoalHandleGoToPoint> goal_handle)
  {
    RCLCPP_INFO(this->get_logger(), "Received request to cancel goal");
    (void)goal_handle;
    return rclcpp_action::CancelResponse::ACCEPT;
  }

  void GoToPoint::handle_accepted(const std::shared_ptr<GoalHandleGoToPoint> goal_handle)
  {
    // this needs to return quickly to avoid blocking the executor, so spin up a new thread
    std::thread{std::bind(&GoToPoint::executeCB, this, _1), goal_handle}.detach();
  }

void GoToPoint::executeCB(const std::shared_ptr<GoalHandleGoToPoint> goal_handle) 
{
    const auto goal = goal_handle->get_goal();
    double Kp = 1.0;

    rclcpp::Rate rate(10);
    while (rclcpp::ok()) {
         // Get the current position
        double current_x = current_pos_.x;
        double current_y = current_pos_.y;

        // Get the desired position
        double desired_x = goal->goal_pos.x;
        double desired_y = goal->goal_pos.y;
        double desired_yaw = goal->goal_pos.z;

        // Compute the difference between the two positions
        double diff_x = desired_x - current_x;
        double diff_y = desired_y - current_y;

        // Calculate the angle (yaw) to the desired position
        //double desired_yaw = std::atan2(diff_y, diff_x);

        // Calculate the difference between the current and desired yaw
        double diff_yaw = desired_yaw - current_pos_.z;

        // Normalize the angle difference to the range [-pi, pi]
        while (diff_yaw > M_PI) 
        {
            diff_yaw -= 2 * M_PI;
        }
        while (diff_yaw < -M_PI) 
        {
            diff_yaw += 2 * M_PI;
        }

        // Calculate the angular speed (angular.z) required
        double angular_speed = diff_yaw * Kp;  // Kp is a proportionality constant

        // Create and publish the velocity command
        geometry_msgs::msg::Twist cmd_vel;
        cmd_vel.linear.x = 0.2;  // Fixed linear.x
        cmd_vel.angular.z = angular_speed;
        cmd_vel_pub_->publish(cmd_vel);

        // Check if the goal is reached
        double distance_to_goal = std::sqrt(diff_x * diff_x + diff_y * diff_y);
        if (distance_to_goal < 0.01) 
        {
            //action_server_->status(goal_handle->get_goal_id());
            cmd_vel.linear.x = 0;  // Fixed linear.x
            cmd_vel.angular.z = 0;
            cmd_vel_pub_->publish(cmd_vel);
            break;
        }
        //rclcpp::spin_some(this);
        rate.sleep();
    }
}