#include <memory>
#include <functional>
#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "my_interfaces/msg/imu_reading.hpp"

// this node listens to both /imu/raw and /imu/filtered and prints a comparison every 1 second
class LoggerNode : public rclcpp::Node
{
public:
  LoggerNode() : Node("logger_node"), has_raw_(false), has_filtered_(false)
  {
    auto qos = rclcpp::QoS(rclcpp::KeepLast(20)).reliable();

    // subscribe to raw IMU data coming from sensor_driver
    sub_raw_ = create_subscription<my_interfaces::msg::ImuReading>(
      "/imu/raw",
      qos,
      std::bind(&LoggerNode::raw_callback, this, std::placeholders::_1)
    );

    // subscribe to filtered IMU data coming from filter_node
    sub_filtered_ = create_subscription<my_interfaces::msg::ImuReading>(
      "/imu/filtered",
      qos,
      std::bind(&LoggerNode::filtered_callback, this, std::placeholders::_1)
    );

    // print a comparison log every 1 second
    timer_ = create_wall_timer(
      std::chrono::milliseconds(1000),
      std::bind(&LoggerNode::timer_callback, this)
    );

    RCLCPP_INFO(get_logger(), "Logger node started");
  }

private:
  void raw_callback(const my_interfaces::msg::ImuReading::SharedPtr msg)
  {
    latest_raw_ = *msg;
    has_raw_ = true;
  }

  void filtered_callback(const my_interfaces::msg::ImuReading::SharedPtr msg)
  {
    latest_filtered_ = *msg;
    has_filtered_ = true;
  }

  void timer_callback()
  {
    // wait until we have received at least one message from each topic
    if (!has_raw_ || !has_filtered_) {
      RCLCPP_WARN(get_logger(), "Waiting for data on both topics...");
      return;
    }

    // diff shows how much the filter is smoothing the signal
    double diff = latest_filtered_.accel_x - latest_raw_.accel_x;

    RCLCPP_INFO(get_logger(),
      "raw accel_x=%.3f | filtered accel_x=%.3f | diff=%.3f",
      latest_raw_.accel_x, latest_filtered_.accel_x, diff);
  }

  rclcpp::Subscription<my_interfaces::msg::ImuReading>::SharedPtr sub_raw_;
  rclcpp::Subscription<my_interfaces::msg::ImuReading>::SharedPtr sub_filtered_;
  rclcpp::TimerBase::SharedPtr timer_;

  my_interfaces::msg::ImuReading latest_raw_;
  my_interfaces::msg::ImuReading latest_filtered_;
  bool has_raw_;
  bool has_filtered_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<LoggerNode>());
  rclcpp::shutdown();
  return 0;
}
