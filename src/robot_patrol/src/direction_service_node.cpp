#include "direction_service.h"


int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  std::shared_ptr<DirectionService> patrol_with_service_node = std::make_shared<DirectionService>();
  rclcpp::spin(patrol_with_service_node);
  rclcpp::shutdown();
  return 0;
}