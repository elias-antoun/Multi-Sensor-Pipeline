cmake_minimum_required(VERSION 3.8)
project(imu_pipeline)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(my_interfaces REQUIRED)

add_executable(sensor_driver src/sensor_driver.cpp)
ament_target_dependencies(sensor_driver rclcpp my_interfaces)

add_executable(filter_node src/filter_node.cpp)
ament_target_dependencies(filter_node rclcpp my_interfaces)

add_executable(logger_node src/logger_node.cpp)
ament_target_dependencies(logger_node rclcpp my_interfaces)

install(TARGETS
  sensor_driver
  filter_node
  logger_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME}
)


ament_package()
