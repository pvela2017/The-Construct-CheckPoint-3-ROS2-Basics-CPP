#include "rclcpp/rclcpp.hpp"
#include "custom_interfaces/srv/get_direction.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "std_msgs/msg/string.hpp"
#include <chrono>
#include <memory>
#include <string>

class Test : public rclcpp::Node {

    public:
        Test() : Node("test") {
            
            rclcpp::SubscriptionOptions sub_thread;
            sub_thread.callback_group = create_callback_group(rclcpp::CallbackGroupType::Reentrant);
            sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>("/scan",10, std::bind(&Test::sub_callback, this, std::placeholders::_1),sub_thread);

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

    private:
        rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr sub_;
        rclcpp::Client<custom_interfaces::srv::GetDirection>::SharedPtr client_;
        std::shared_ptr<custom_interfaces::srv::GetDirection::Request> request_;

        void sub_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg) 
        {
            auto request_ =std::make_shared<custom_interfaces::srv::GetDirection::Request>();
            request_->laser_data = *msg;
            client_->async_send_request(request_, std::bind(&Test::service_response, this, std::placeholders::_1));
        }

        void service_response(rclcpp::Client<custom_interfaces::srv::GetDirection>::SharedFuture futuro)
        {
            auto status = futuro.wait_for(std::chrono::seconds(1));
            if(status == std::future_status::ready)
            {
                auto response = futuro.get();
                RCLCPP_INFO(this->get_logger(), "Service was called!");
                RCLCPP_INFO(this->get_logger(), "Recevied direction: %s", response->direction.c_str());
            }
            else 
            {
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service!");
            }
        }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<Test>());
    rclcpp::shutdown();
  return 0;
}