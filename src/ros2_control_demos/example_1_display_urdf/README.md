# 1. example 1 display urdf

- [1. example 1 display urdf](#1-example-1-display-urdf)
  - [1.1. 编译 ros2\_control\_demo\_example\_1\_display\_urdf](#11-编译-ros2_control_demo_example_1_display_urdf)
  - [1.2. 运行 ros2\_control\_demo\_example\_1\_display\_urdf](#12-运行-ros2_control_demo_example_1_display_urdf)

此包的目的是:

- 练习使用rviz显示机器人URDF
- 通过joint_state_publisher_gui移动机器人的可动关节

## 1.1. 编译 ros2_control_demo_example_1_display_urdf

```bash
colcon build --packages-select ros2_control_demo_example_1_display_urdf --symlink-install
```

## 1.2. 运行 ros2_control_demo_example_1_display_urdf

```bash
source install/setup.bash
ros2 launch ros2_control_demo_example_1_display_urdf view_robot.launch.py
```
