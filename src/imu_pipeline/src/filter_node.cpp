#include <memory>
#include <functional>
#include <deque>
#include "rclcpp/rclcpp.hpp"
#include "my_interfaces/msg/imu_reading.hpp"

class FilterNode : public rclcpp::Node
{
public:
  FilterNode() : Node("filter_node")
  {
    declare_parameter("window_size", 10);
    window_size_ = get_parameter("window_size").as_int();

    auto qos = rclcpp::QoS(rclcpp::KeepLast(20)).reliable();

    subscription_ = create_subscription<my_interfaces::msg::ImuReading>(
      "/imu/raw",
      qos,
      std::bind(&FilterNode::topic_callback, this, std::placeholders::_1)
    );

    publisher_ = create_publisher<my_interfaces::msg::ImuReading>(
      "/imu/filtered",
      qos
    );

    RCLCPP_INFO(get_logger(), "Filter node started with window_size=%d", window_size_);
  }

private:
  // we modify history_ every call
  void topic_callback(const my_interfaces::msg::ImuReading::SharedPtr msg)
  {
    history_.push_back(*msg);
    if (static_cast<int>(history_.size()) > window_size_) {
      history_.pop_front();
    }

    double avg_accel_x = 0.0, avg_accel_y = 0.0, avg_accel_z = 0.0;
    double avg_gyro_x  = 0.0, avg_gyro_y  = 0.0, avg_gyro_z  = 0.0;

    for (const auto & r : history_) {
      avg_accel_x += r.accel_x;
      avg_accel_y += r.accel_y;
      avg_accel_z += r.accel_z;
      avg_gyro_x  += r.gyro_x;
      avg_gyro_y  += r.gyro_y;
      avg_gyro_z  += r.gyro_z;
    }

    double n = static_cast<double>(history_.size());

    my_interfaces::msg::ImuReading filtered_msg;
    filtered_msg.accel_x = avg_accel_x / n;
    filtered_msg.accel_y = avg_accel_y / n;
    filtered_msg.accel_z = avg_accel_z / n;
    filtered_msg.gyro_x  = avg_gyro_x  / n;
    filtered_msg.gyro_y  = avg_gyro_y  / n;
    filtered_msg.gyro_z  = avg_gyro_z  / n;
    filtered_msg.stamp   = msg->stamp;

    publisher_->publish(filtered_msg);

    RCLCPP_INFO(get_logger(), "Filtered accel: [%.3f, %.3f, %.3f]",
      filtered_msg.accel_x, filtered_msg.accel_y, filtered_msg.accel_z);
  }

  rclcpp::Subscription<my_interfaces::msg::ImuReading>::SharedPtr subscription_;
  rclcpp::Publisher<my_interfaces::msg::ImuReading>::SharedPtr publisher_;
  std::deque<my_interfaces::msg::ImuReading> history_;  // persists between callbacks
  int window_size_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<FilterNode>());
  rclcpp::shutdown();
  return 0;
}
