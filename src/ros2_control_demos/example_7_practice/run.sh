#!/bin/bash
set -e

echo "===== 清理 build/ install/ log/ 目录 ====="
rm -rf build/ install/ log/

echo "===== 编译 ====="
colcon build --packages-select r6bot_control_system

echo "===== 设置环境变量 ====="
source install/setup.bash

echo "===== 启动 ====="
# ros2 launch r6bot_control_system view_r6bot.launch.py
ros2 launch r6bot_control_system r6bot_control_system.launch.py

echo "===== 结束 ====="