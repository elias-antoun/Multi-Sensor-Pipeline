#include <chrono>
#include <functional>
#include <memory>
#include <cmath>
#include <cstdlib>
#include "rclcpp/rclcpp.hpp"
#include "my_interfaces/msg/imu_reading.hpp"

class SensorDriver : public rclcpp::Node
{
public:
  SensorDriver() : Node("sensor_driver"), count_(0)
  {
    declare_parameter<double>("publish_rate_hz", 50.0);
    double rate_hz = get_parameter("publish_rate_hz").as_double();

    RCLCPP_INFO(get_logger(), "Sensor driver starting at %.2f Hz", rate_hz);

    publisher_ = create_publisher<my_interfaces::msg::ImuReading>(
      "/imu/raw",
      rclcpp::QoS(10).reliable()
    );

    auto interval_ms = std::chrono::milliseconds(static_cast<int>(1000.0 / rate_hz));
    timer_ = create_wall_timer(interval_ms, std::bind(&SensorDriver::timer_callback, this));
  }

private:
  void timer_callback()
  {
    auto msg = my_interfaces::msg::ImuReading();

    // sinusoid on accelerometer fields so the output visibly changes over time
    double t = count_ * 0.02;
    msg.accel_x = std::sin(t);
    msg.accel_y = std::sin(t + 1.0);
    msg.accel_z = 9.8 + 0.1 * std::sin(t + 2.0);

    // random noise on gyroscope fields
    msg.gyro_x = (std::rand() % 100 - 50) / 1000.0;
    msg.gyro_y = (std::rand() % 100 - 50) / 1000.0;
    msg.gyro_z = (std::rand() % 100 - 50) / 1000.0;

    msg.stamp = now();

    RCLCPP_INFO(get_logger(), "Publishing accelerometer: [%.3f, %.3f, %.3f]",
      msg.accel_x, msg.accel_y, msg.accel_z);

    publisher_->publish(msg);
    count_++;
  }

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<my_interfaces::msg::ImuReading>::SharedPtr publisher_;
  int count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<SensorDriver>());
  rclcpp::shutdown();
  return 0;
}
