from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    config = os.path.join(
        get_package_share_directory('imu_pipeline'),
        'config',
        'imu_pipeline.yaml'
    )

    return LaunchDescription([
        Node(
            package='imu_pipeline',
            executable='sensor_driver',
            name='sensor_driver',
            namespace='imu',
            parameters=[config],
            output='screen'
        ),
        Node(
            package='imu_pipeline',
            executable='filter_node',
            name='filter_node',
            namespace='imu',
            parameters=[config],
            output='screen'
        ),
        Node(
            package='imu_pipeline',
            executable='logger_node',
            name='logger_node',
            namespace='imu',
            parameters=[config],
            output='screen'
        ),
    ])
