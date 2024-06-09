cmake_minimum_required(VERSION 3.5)
project(robot_patrol)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(custom_interfaces REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(action_msgs REQUIRED)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

rosidl_generate_interfaces(${PROJECT_NAME}
  "action/GoToPoint.action"
  DEPENDENCIES std_msgs sensor_msgs geometry_msgs # Add it when you use other dependency in the file 
)

include_directories(include)

add_executable(patrol_node src/patrol_node.cpp
				src/patrol.cpp)
ament_target_dependencies(patrol_node rclcpp sensor_msgs geometry_msgs)

add_executable(direction_service_node src/direction_service_node.cpp
				src/direction_service.cpp)
ament_target_dependencies(direction_service_node rclcpp sensor_msgs std_msgs custom_interfaces)

add_executable(test_node src/test.cpp)
ament_target_dependencies(test_node rclcpp sensor_msgs geometry_msgs custom_interfaces std_msgs)

add_executable(patrol_with_service_node src/patrol_with_service_node.cpp
                 src/patrol_with_service.cpp)
ament_target_dependencies(patrol_with_service_node rclcpp sensor_msgs geometry_msgs custom_interfaces std_msgs)

add_executable(go_to_point_action_node src/go_to_point_action_node.cpp
                 src/go_to_point_action.cpp)
ament_target_dependencies(go_to_point_action_node rclcpp rclcpp_action sensor_msgs geometry_msgs std_msgs nav_msgs)
rosidl_target_interfaces(go_to_point_action_node ${PROJECT_NAME} "rosidl_typesupport_cpp")

install(TARGETS
   patrol_node
   direction_service_node
   patrol_with_service_node
   go_to_point_action_node
   test_node
   DESTINATION lib/${PROJECT_NAME}
 )

# Install launch files.
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}/
)

install(
  DIRECTORY include/
  DESTINATION include
)

ament_package()
