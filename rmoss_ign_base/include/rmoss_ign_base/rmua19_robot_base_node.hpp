// Copyright 2021 RoboMaster-OSS
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

#ifndef RMOSS_IGN_BASE__RMUA19_ROBOT_BASE_NODE_HPP_
#define RMOSS_IGN_BASE__RMUA19_ROBOT_BASE_NODE_HPP_

#include <thread>
#include <memory>
#include "rclcpp/rclcpp.hpp"

#include "rmoss_ign_base/chassis_controller.hpp"
#include "rmoss_ign_base/gimbal_controller.hpp"
#include "rmoss_ign_base/shooter_controller.hpp"
#include "rmoss_ign_base/gimbal_state_publisher.hpp"
#include "rmoss_ign_base/odometry_publisher.hpp"
#include "rmoss_ign_base/pid.hpp"

namespace rmoss_ign_base
{
// Node wrapper for Rmua19RobotBaseNode
class Rmua19RobotBaseNode
{
public:
  explicit Rmua19RobotBaseNode(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

public:
  rclcpp::node_interfaces::NodeBaseInterface::SharedPtr get_node_base_interface()
  {
    return node_->get_node_base_interface();
  }

private:
  rclcpp::Node::SharedPtr node_;
  std::shared_ptr<ignition::transport::Node> ign_node_;
  // ign moudule
  std::shared_ptr<rmoss_ign_base::IgnJointEncoder> ign_gimbal_encoder_;
  std::shared_ptr<rmoss_ign_base::IgnImu> ign_gimbal_imu_;
  std::shared_ptr<rmoss_ign_base::IgnChassisCmd> ign_chassis_cmd_;
  std::shared_ptr<rmoss_ign_base::IgnGimbalCmd> ign_gimbal_cmd_;
  // ros controller/publisher wrapper
  std::shared_ptr<rmoss_ign_base::ChassisController> chassis_controller_;
  std::shared_ptr<rmoss_ign_base::GimbalController> gimbal_controller_;
  std::shared_ptr<rmoss_ign_base::ShooterController> shooter_controller_;
  std::shared_ptr<rmoss_ign_base::GimbalStatePublisher> gimbal_publisher_;
  std::shared_ptr<rmoss_ign_base::OdometryPublisher> odometry_publisher_;
};

}  // namespace rmoss_ign_base

#endif  // RMOSS_IGN_BASE__RMUA19_ROBOT_BASE_NODE_HPP_