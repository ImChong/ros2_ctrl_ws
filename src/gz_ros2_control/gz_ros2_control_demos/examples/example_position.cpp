// Copyright 2021 Open Source Robotics Foundation, Inc.
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

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>

#include <control_msgs/action/follow_joint_trajectory.hpp>

std::shared_ptr<rclcpp::Node> node;
bool common_goal_accepted = false;
rclcpp_action::ResultCode common_resultcode = rclcpp_action::ResultCode::UNKNOWN;
int common_action_result_code = control_msgs::action::FollowJointTrajectory_Result::SUCCESSFUL;

std::vector<double> desired_goals = {0, -1, 1, 0.0};
unsigned int ct_goals_reached = 0;

void common_goal_response(
  rclcpp_action::ClientGoalHandle
  <control_msgs::action::FollowJointTrajectory>::SharedPtr future)
{
  RCLCPP_DEBUG(
    node->get_logger(), "common_goal_response time: %f",
    rclcpp::Clock().now().seconds());
  auto goal_handle = future.get();
  if (!goal_handle) {
    common_goal_accepted = false;
    printf("Goal rejected\n");
  } else {
    common_goal_accepted = true;
    printf("Goal accepted\n");
  }
}

void common_result_response(
  const rclcpp_action::ClientGoalHandle
  <control_msgs::action::FollowJointTrajectory>::WrappedResult & result)
{
  printf("common_result_response time: %f\n", rclcpp::Clock(RCL_ROS_TIME).now().seconds());
  common_resultcode = result.code;
  common_action_result_code = result.result->error_code;
  switch (result.code) {
    case rclcpp_action::ResultCode::SUCCEEDED:
      printf("Action goal succeeded\n");
      break;
    case rclcpp_action::ResultCode::ABORTED:
      printf("Action goal was aborted\n");
      return;
    case rclcpp_action::ResultCode::CANCELED:
      printf("Action goal was canceled\n");
      return;
    default:
      printf("Action goal: Unknown result code\n");
      return;
  }
}

void common_feedback(
  rclcpp_action::ClientGoalHandle<control_msgs::action::FollowJointTrajectory>::SharedPtr,
  const std::shared_ptr<const control_msgs::action::FollowJointTrajectory::Feedback> feedback)
{
  std::cout << "feedback->desired: positions: " << feedback->desired.positions.at(0);
  std::cout << ", velocities: " << feedback->desired.velocities.at(0) << std::endl;

  std::cout << "feedback->actual: positions: " << feedback->actual.positions.at(0);
  std::cout << std::endl;

  if (ct_goals_reached < desired_goals.size()) {
    if (fabs(feedback->actual.positions[0] - desired_goals.at(ct_goals_reached)) < 0.1) {
      std::cout << "Goal # " << ct_goals_reached << ": " << desired_goals.at(ct_goals_reached) <<
        " reached" << std::endl;
      ct_goals_reached++;
      if (ct_goals_reached < desired_goals.size()) {
        std::cout << "next goal # " << ct_goals_reached << ": " <<
          desired_goals.at(ct_goals_reached) <<
          std::endl;
      }
    }
  }
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  node = std::make_shared<rclcpp::Node>("trajectory_test_node");

  RCLCPP_DEBUG(node->get_logger(), "node created");

  rclcpp_action::Client<control_msgs::action::FollowJointTrajectory>::SharedPtr action_client;
  action_client = rclcpp_action::create_client<control_msgs::action::FollowJointTrajectory>(
    node->get_node_base_interface(),
    node->get_node_graph_interface(),
    node->get_node_logging_interface(),
    node->get_node_waitables_interface(),
    "/joint_trajectory_controller/follow_joint_trajectory");

  while (true) {
    bool response =
      action_client->wait_for_action_server(std::chrono::seconds(1));
    if (!response) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(2000ms);
      RCLCPP_WARN(node->get_logger(), "Trying to connect to the server again");
      continue;
    } else {
      break;
    }
  }

  RCLCPP_DEBUG(node->get_logger(), "Created action server");

  std::vector<std::string> joint_names = {"slider_to_cart"};

  std::vector<trajectory_msgs::msg::JointTrajectoryPoint> points;
  trajectory_msgs::msg::JointTrajectoryPoint point;
  point.time_from_start = rclcpp::Duration::from_seconds(1.0);
  point.positions.resize(joint_names.size());
  point.positions[0] = desired_goals[0];

  trajectory_msgs::msg::JointTrajectoryPoint point2;
  point2.time_from_start = rclcpp::Duration::from_seconds(2.0);
  point2.positions.resize(joint_names.size());
  point2.positions[0] = desired_goals[1];

  trajectory_msgs::msg::JointTrajectoryPoint point3;
  point3.time_from_start = rclcpp::Duration::from_seconds(3.0);
  point3.positions.resize(joint_names.size());
  point3.positions[0] = desired_goals[2];

  trajectory_msgs::msg::JointTrajectoryPoint point4;
  point4.time_from_start = rclcpp::Duration::from_seconds(4.0);
  point4.positions.resize(joint_names.size());
  point4.positions[0] = desired_goals[3];

  points.push_back(point);
  points.push_back(point2);
  points.push_back(point3);
  points.push_back(point4);

  rclcpp_action::Client<control_msgs::action::FollowJointTrajectory>::SendGoalOptions opt;
  opt.goal_response_callback = std::bind(common_goal_response, std::placeholders::_1);
  opt.result_callback = std::bind(common_result_response, std::placeholders::_1);
  opt.feedback_callback = std::bind(common_feedback, std::placeholders::_1, std::placeholders::_2);

  control_msgs::action::FollowJointTrajectory_Goal goal_msg;
  goal_msg.goal_time_tolerance = rclcpp::Duration::from_seconds(1.0);
  goal_msg.trajectory.joint_names = joint_names;
  goal_msg.trajectory.points = points;

  RCLCPP_DEBUG(node->get_logger(), "async_send_goal");
  auto goal_handle_future = action_client->async_send_goal(goal_msg, opt);

  if (rclcpp::spin_until_future_complete(node, goal_handle_future) !=
    rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_ERROR(node->get_logger(), "send goal call failed :(");
    action_client.reset();
    node.reset();
    return 1;
  }
  RCLCPP_INFO(node->get_logger(), "send goal call ok :)");

  rclcpp_action::ClientGoalHandle<control_msgs::action::FollowJointTrajectory>::SharedPtr
    goal_handle = goal_handle_future.get();
  if (!goal_handle) {
    RCLCPP_ERROR(node->get_logger(), "Goal was rejected by server");
    action_client.reset();
    node.reset();
    return 1;
  }
  RCLCPP_INFO(node->get_logger(), "Goal was accepted by server");

  // Wait for the server to be done with the goal
  auto result_future = action_client->async_get_result(goal_handle);
  RCLCPP_INFO(node->get_logger(), "Waiting for result");
  if (rclcpp::spin_until_future_complete(node, result_future) !=
    rclcpp::FutureReturnCode::SUCCESS)
  {
    RCLCPP_ERROR(node->get_logger(), "get result call failed :(");
    action_client.reset();
    node.reset();
    return 1;
  }
  action_client.reset();

  if (desired_goals.size() != ct_goals_reached) {
    RCLCPP_ERROR(node->get_logger(), "Not all the goals were reached");
    rclcpp::shutdown();
    return -1;
  }

  node.reset();

  rclcpp::shutdown();

  return 0;
}
