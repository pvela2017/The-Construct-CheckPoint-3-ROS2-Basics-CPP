#include "patrol_with_service.h"

Patrol::Patrol() : Node("patrol_with_service_node") 
{
  callback_group_ = this->create_callback_group(rclcpp::CallbackGroupType::Reentrant);

  rclcpp::SubscriptionOptions options1;
  options1.callback_group = callback_group_;

  subscription_laserscan_ = this->create_subscription<sensor_msgs::msg::LaserScan>("/scan", 10, std::bind(&Patrol::laserscanCallBack, this, std::placeholders::_1), options1);
  publisher_cmdvel_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
  timer_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&Patrol::cmdvelCallBack, this), callback_group_);

  client_ = this->create_client<custom_interfaces::srv::GetDirection>("/direction_service");      

    while (!client_->wait_for_service(std::chrono::seconds(5))) 
    {
        if (!rclcpp::ok()) 
        {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service. Exiting.");
            return;
        }
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"),
            "Service not available, waiting again...");
    }
}

Patrol::~Patrol() {}

void Patrol::laserscanCallBack(const sensor_msgs::msg::LaserScan::SharedPtr msg) 
{
  last_laser_ = *msg;
  
}

void Patrol::cmdvelCallBack() 
{
  auto request = std::make_shared<custom_interfaces::srv::GetDirection::Request>();
  request->laser_data = last_laser_;
  client_->async_send_request(request, std::bind(&Patrol::serviceResponse, this, std::placeholders::_1));
}

void Patrol::serviceResponse(rclcpp::Client<custom_interfaces::srv::GetDirection>::SharedFuture future_result) 
{
  auto message = geometry_msgs::msg::Twist();
  auto response = future_result.get();

  message.linear.y = 0;
  message.linear.z = 0;
  message.angular.x = 0;
  message.angular.y = 0;

    if (response->direction == "left") 
    {
        message.linear.x = 0.1;
        message.angular.z = 0.5;
        publisher_cmdvel_->publish(message);
    }

    else if (response->direction == "forward") 
    {
        message.linear.x = 0.1;
        message.angular.z = 0.0;
        publisher_cmdvel_->publish(message);
    }

    else if (response->direction == "right") 
    {
        message.linear.x = 0.1;
        message.angular.z = -0.5;
        publisher_cmdvel_->publish(message);
    }
    else
    {
        std::cout << response->direction;
    }
  
}