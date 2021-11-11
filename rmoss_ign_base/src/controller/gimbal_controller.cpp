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

#include "rmoss_ign_base/gimbal_controller.hpp"

#include <memory>
#include <string>
#include <algorithm>

namespace rmoss_ign_base
{

GimbalController::GimbalController(
  rclcpp::Node::SharedPtr node,
  std::shared_ptr<IgnGimbalCmd> ign_gimbal_cmd,
  std::shared_ptr<IgnJointEncoder> ign_gimbal_encoder,
  std::shared_ptr<IgnImu> ign_gimbal_imu,
  const std::string & gimbal_name)
{
  // set handler
  node_ = node;
  ign_gimbal_cmd_ = ign_gimbal_cmd;
  ign_gimbal_encoder_ = ign_gimbal_encoder;
  ign_gimbal_imu_ = ign_gimbal_imu;
  // ros sub
  using namespace std::placeholders;
  std::string ros_gimbal_cmd_topic = "robot_base/gimbal_cmd";
  if (gimbal_name != "") {
    ros_gimbal_cmd_topic = "robot_base/" + gimbal_name + "/gimbal_cmd";
  }
  ros_gimbal_cmd_sub_ = node_->create_subscription<rmoss_interfaces::msg::GimbalCmd>(
    ros_gimbal_cmd_topic, 10, std::bind(&GimbalController::gimbal_cb, this, _1));
  // timer and set_parameters callback
  auto period = std::chrono::milliseconds(10);
  controller_timer_ = node_->create_wall_timer(
    period, std::bind(&GimbalController::update, this));
}

void GimbalController::update()
{
  auto dt = std::chrono::milliseconds(10);
  // pid for pitch
  double pitch_err = ign_gimbal_imu_->get_pitch() - target_pitch_;
  double pitch_cmd = picth_pid_.Update(pitch_err, dt);
  // pid for yaw
  double yaw_err = ign_gimbal_imu_->get_yaw() - target_yaw_;
  double yaw_cmd = yaw_pid_.Update(yaw_err, dt);
  // printf("imu:%lf,%lf\n",ign_gimbal_imu_->get_yaw() ,ign_gimbal_imu_->get_pitch());
  // printf("imu:%lf,%lf\n",ign_gimbal_imu_->get_yaw() ,target_yaw_);
  // publish CMD
  ign_gimbal_cmd_->publish(pitch_cmd, yaw_cmd);
}


void GimbalController::gimbal_cb(const rmoss_interfaces::msg::GimbalCmd::SharedPtr msg)
{
  // for pitch
  if (msg->pitch_type == msg->ABSOLUTE_ANGLE) {
    target_pitch_ = msg->position.pitch;
  } else if (msg->pitch_type == msg->RELATIVE_ANGLE) {
    target_pitch_ = ign_gimbal_imu_->get_pitch() + msg->position.pitch;
  } else {
    RCLCPP_WARN(node_->get_logger(), "pitch type[%d] isn't supported!", msg->pitch_type);
  }
  // limitation for pitch
  target_pitch_ = std::min(target_pitch_, 1.0);
  target_pitch_ = std::max(target_pitch_, -1.0);
  // for yaw
  if (msg->yaw_type == msg->ABSOLUTE_ANGLE) {
    target_yaw_ = msg->position.yaw;
  } else if (msg->yaw_type == msg->RELATIVE_ANGLE) {
    target_yaw_ = ign_gimbal_imu_->get_yaw() + msg->position.yaw;
  } else {
    RCLCPP_WARN(node_->get_logger(), "yaw type[%d] isn't supported!", msg->yaw_type);
  }
}

void GimbalController::set_yaw_pid(struct PidParam pid_param)
{
  yaw_pid_.Init(
    pid_param.p, pid_param.i, pid_param.d, pid_param.imax,
    pid_param.imin, pid_param.cmdmax, pid_param.cmdmin, pid_param.offset);
}
void GimbalController::set_pitch_pid(struct PidParam pid_param)
{
  picth_pid_.Init(
    pid_param.p, pid_param.i, pid_param.d, pid_param.imax,
    pid_param.imin, pid_param.cmdmax, pid_param.cmdmin, pid_param.offset);
}

}  // namespace rmoss_ign_base