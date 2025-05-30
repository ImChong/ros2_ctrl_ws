// Copyright 2020 ros2_control Development Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ROS2_CONTROL_DEMO_EXAMPLE_8__RRBOT_TRANSMISSIONS_SYSTEM_POSITION_ONLY_HPP_
#define ROS2_CONTROL_DEMO_EXAMPLE_8__RRBOT_TRANSMISSIONS_SYSTEM_POSITION_ONLY_HPP_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "rclcpp/clock.hpp"
#include "rclcpp/logger.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp_lifecycle/state.hpp"
#include "transmission_interface/transmission.hpp"

namespace ros2_control_demo_example_8
{
class RRBotTransmissionsSystemPositionOnlyHardware : public hardware_interface::SystemInterface
{
public:
  hardware_interface::CallbackReturn on_init(
    const hardware_interface::HardwareInfo & info) override;

  hardware_interface::CallbackReturn on_configure(
    const rclcpp_lifecycle::State & previous_state) override;

  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

  std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

  hardware_interface::CallbackReturn on_activate(
    const rclcpp_lifecycle::State & previous_state) override;

  hardware_interface::CallbackReturn on_deactivate(
    const rclcpp_lifecycle::State & previous_state) override;

  hardware_interface::return_type read(
    const rclcpp::Time & time, const rclcpp::Duration & period) override;

  hardware_interface::return_type write(
    const rclcpp::Time & time, const rclcpp::Duration & period) override;

  /// Get the logger of the SystemInterface.
  /**
   * \return logger of the SystemInterface.
   */
  rclcpp::Logger get_logger() const { return *logger_; }

  /// Get the clock of the SystemInterface.
  /**
   * \return clock of the SystemInterface.
   */
  rclcpp::Clock::SharedPtr get_clock() const { return clock_; }

private:
  // parameters for the RRBot simulation
  double actuator_slowdown_;

  // transmissions
  std::vector<std::shared_ptr<transmission_interface::Transmission>> transmissions_;

  struct InterfaceData
  {
    explicit InterfaceData(const std::string & name);

    std::string name_;
    double command_;
    double state_;

    // this is the "sink" that will be part of the transmission Joint/Actuator handles
    double transmission_passthrough_;
  };
  std::vector<InterfaceData> joint_interfaces_;
  std::vector<InterfaceData> actuator_interfaces_;

  // Objects for logging
  std::shared_ptr<rclcpp::Logger> logger_;
  rclcpp::Clock::SharedPtr clock_;
};

}  // namespace ros2_control_demo_example_8

#endif  // ROS2_CONTROL_DEMO_EXAMPLE_8__RRBOT_TRANSMISSIONS_SYSTEM_POSITION_ONLY_HPP_
