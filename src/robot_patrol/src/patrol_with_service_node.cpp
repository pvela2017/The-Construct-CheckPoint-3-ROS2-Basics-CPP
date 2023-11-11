#include "patrol_with_service.h"

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);

  std::shared_ptr<Patrol> patrol_with_service_node = std::make_shared<Patrol>();

  // Initialize one MultiThreadedExecutor object
  rclcpp::executors::MultiThreadedExecutor executor;
  executor.add_node(patrol_with_service_node);
  executor.spin();

  rclcpp::shutdown();
  return 0;
}