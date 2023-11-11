#include "patrol.h"

Patrol::Patrol() : Node("patrol_node") 
{
  // rcutils_logging_set_logger_level(node->get_logger().get_name(),
  // RCUTILS_LOG_SEVERITY_DEBUG);
  callback_group_ = this->create_callback_group(rclcpp::CallbackGroupType::Reentrant);

  rclcpp::SubscriptionOptions options1;
  options1.callback_group = callback_group_;

  subscription_laserscan_ = this->create_subscription<sensor_msgs::msg::LaserScan>("/scan", 10, std::bind(&Patrol::laserscanCallBack, this, std::placeholders::_1), options1);
  publisher_cmdvel_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
  timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&Patrol::cmdvelCallBack, this), callback_group_);
}

Patrol::~Patrol() {}

void Patrol::laserscanCallBack(const sensor_msgs::msg::LaserScan::SharedPtr msg) 
{
  double min_angle = -M_PI / 2;
  double max_angle = M_PI / 2;
  double clear_distance = 0.3;


  double minus_pi_2_index = int((min_angle - msg->angle_min) / msg->angle_increment);
  double pi_2_index       = int((max_angle - msg->angle_min) / msg->angle_increment);

  if (minus_pi_2_index > msg->ranges.size() || pi_2_index > msg->ranges.size()) 
  {
    RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Index out of size.");
  } 
  else 
  {
    for (int i = minus_pi_2_index; i <= pi_2_index; i++) 
    {
      if (msg->ranges[i] > clear_distance && !std::isnan(msg->ranges[i]) && !std::isinf(msg->ranges[i]) && msg->ranges[i] < 1) 
      {
        clear_distance = msg->ranges[i];
        direction_ = msg->angle_min + (i) * msg->angle_increment;
        //std::cout << i << "\n";
      }
    }
  }
}

void Patrol::cmdvelCallBack() 
{
  auto message = geometry_msgs::msg::Twist();
  message.linear.x = 0.1;
  message.linear.y = 0;
  message.linear.z = 0;

  message.angular.x = 0;
  message.angular.y = 0;
  message.angular.z = direction_ / 2;

  publisher_cmdvel_->publish(message);
}