#include "go_to_point_action.h"

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    auto go_to_point_node = std::make_shared<GoToPoint>();
    // Initialize one MultiThreadedExecutor object
    rclcpp::executors::MultiThreadedExecutor executor;
    executor.add_node(go_to_point_node);
    executor.spin();

    rclcpp::shutdown();
    return 0;

}