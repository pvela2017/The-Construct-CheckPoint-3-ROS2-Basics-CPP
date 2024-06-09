#include "rclcpp/rclcpp.hpp"
#include "custom_interfaces/srv/get_direction.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "std_msgs/msg/string.hpp"
#include <chrono>
#include <memory>

using GetDirection = custom_interfaces::srv::GetDirection;
using std::placeholders::_1;
using std::placeholders::_2;

class DirectionService : public rclcpp::Node
{
public:
  DirectionService();
  ~DirectionService();

private:
  rclcpp::Service<GetDirection>::SharedPtr srv_;
  void directionCallBack(const std::shared_ptr<GetDirection::Request>, const std::shared_ptr<GetDirection::Response>);
};
