#include "geometry_msgs/msg/detail/twist__struct.hpp"
#include "rclcpp/rclcpp.hpp"
#include "custom_interfaces/srv/get_direction.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <chrono>

class Patrol : public rclcpp::Node
{
public:
    Patrol();
    ~Patrol();
private:
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_laserscan_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_cmdvel_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::CallbackGroup::SharedPtr callback_group_;

    rclcpp::Client<custom_interfaces::srv::GetDirection>::SharedPtr client_;

    void cmdvelCallBack();
    void laserscanCallBack(const sensor_msgs::msg::LaserScan::SharedPtr);
    void serviceResponse(rclcpp::Client<custom_interfaces::srv::GetDirection>::SharedFuture);
    sensor_msgs::msg::LaserScan last_laser_;
};