# Multi-Sensor IMU Pipeline — ROS 2 Jazzy

A 3-node ROS 2 pipeline that simulates IMU sensor data, applies a moving average filter, and logs a raw vs filtered comparison every second.

---

## Pipeline Overview

```
sensor_driver --> /imu/raw --> filter_node --> /imu/filtered --> logger_node
                     \_______________________________________________/
                          (logger_node also reads /imu/raw directly)
```

| Node | What it does |
|---|---|
| `sensor_driver` | Publishes simulated IMU data on `/imu/raw` at 50 Hz |
| `filter_node` | Applies a per-axis moving average and publishes to `/imu/filtered` |
| `logger_node` | Subscribes to both topics and prints a raw vs filtered comparison every 1s |

---

## Prerequisites

- ROS 2 Jazzy installed and sourced
- `my_interfaces` package (included in this repo under `src/my_interfaces`)

---

## Build

```bash
cd ~/ros2_ws
source /opt/ros/jazzy/setup.bash
colcon build --symlink-install --packages-select my_interfaces imu_pipeline
source install/setup.bash
```

---

## Run

```bash
ros2 launch imu_pipeline imu_pipeline.launch.py
```

You should see `sensor_driver` publishing at 50 Hz, `filter_node` printing filtered values, and `logger_node` printing a comparison every second.

---

## Verify

**1. Check topic frequency (should be ~50 Hz):**
```bash
ros2 topic hz /imu/raw
```

**2. Check parameters loaded from YAML:**
```bash
ros2 param list
```
You should see all three nodes under `/imu/` with their parameters:
- `/imu/sensor_driver` → `publish_rate_hz`
- `/imu/filter_node` → `window_size`

Confirm the actual values match the YAML:
```bash
ros2 param get /imu/sensor_driver publish_rate_hz
ros2 param get /imu/filter_node window_size
```
Expected: `50.0` and `20`.

**3. Verify the moving average works by changing window_size at runtime:**

With the pipeline running, open a second terminal and run:

```bash
# small window — filtered tracks raw closely, diff is small
ros2 param set /imu/filter_node window_size 3
```
Watch the logger output — the difference between raw and filtered accel_x should be very small.

```bash
# large window — heavy smoothing, filtered responds slowly, diff is large
ros2 param set /imu/filter_node window_size 50
```
The difference should grow noticeably because the filter is now averaging over 50 samples.

This confirms the moving average responds correctly to the parameter — no rebuild needed, just change the value and observe.

---

## Configuration

All parameters are in `src/imu_pipeline/config/imu_pipeline.yaml`:

```yaml
sensor_driver:
  ros__parameters:
    publish_rate_hz: 50.0

filter_node:
  ros__parameters:
    window_size: 20

logger_node:
  ros__parameters: {}
```

Change `window_size` to a smaller value (e.g. 3) to see less smoothing, or larger (e.g. 50) to see more.

---

## Package Structure

```
src/
├── my_interfaces/          # custom ImuReading.msg
│   └── msg/ImuReading.msg
└── imu_pipeline/
    ├── src/
    │   ├── sensor_driver.cpp
    │   ├── filter_node.cpp
    │   └── logger_node.cpp
    ├── launch/
    │   └── imu_pipeline.launch.py
    └── config/
        └── imu_pipeline.yaml
```
