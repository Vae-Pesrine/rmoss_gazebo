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

#include "rmoss_ign_base/ign_joint_encoder.hpp"

#include <memory>
#include <string>
#include <cmath>


namespace rmoss_ign_base
{

IgnJointEncoder::IgnJointEncoder(
  rclcpp::Node::SharedPtr node,
  const std::shared_ptr<ignition::transport::Node> & ign_node,
  const std::string & ign_joint_state_topic)
{
  ign_node_ = ign_node;
  ign_node_->Subscribe(ign_joint_state_topic, &IgnJointEncoder::ign_Joint_state_cb, this);
}

void IgnJointEncoder::ign_Joint_state_cb(const ignition::msgs::Model & msg)
{
  joint_state_msg_ = msg;
  // init index of joints in joint_state.
  if (!is_init_) {
    for (int i = 0; i < msg.joint_size(); i++) {
      joint_names_.insert(msg.joint(i).name());
      joint_position_sensors_[msg.joint(i).name()] = std::make_shared<DataSensor<double>>();
    }
    is_init_ = true;
  }
  for (int i = 0; i < msg.joint_size(); i++) {
    joint_position_sensors_[msg.joint(i).name()]->update(msg.joint(i).axis1().position(), node_->get_clock()->now());
  }
}


IgnGimbalEncoder::IgnGimbalEncoder(
    rclcpp::Node::SharedPtr node,
    std::shared_ptr<ignition::transport::Node> ign_node,
    const std::string & ign_joint_state_topic)
: node_(node), ign_node_(ign_node)
{
  ign_node_->Subscribe(ign_joint_state_topic, &IgnGimbalEncoder::ign_Joint_state_cb, this);
}

void IgnGimbalEncoder::ign_Joint_state_cb(const ignition::msgs::Model & msg)
{
  rmoss_interfaces::msg::Gimbal position, velocity;
  for (int i = 0; i < msg.joint_size(); i++) {
    if (msg.joint(i).name().find("pitch") != std::string::npos) {
      position.pitch = msg.joint(i).axis1().position();
      velocity.pitch = msg.joint(i).axis1().velocity();
    }
    if (msg.joint(i).name().find("yaw") != std::string::npos) {
      position.yaw = msg.joint(i).axis1().position();
      velocity.yaw = msg.joint(i).axis1().velocity();
    }
  }
  position_sensor_->update(position, node_->get_clock()->now());
  velocity_sensor_->update(velocity, node_->get_clock()->now());
}




}  // namespace rmoss_ign_base