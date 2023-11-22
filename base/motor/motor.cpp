//
// Created by 98383 on 2023/11/20.
//

#include "motor.h"
#include "../common/math.h"

namespace motor{
  const uint32_t connect_time_out_ = 50;
}

Motor::Motor(const Motor::Type_e &type, const float &ratio,
             const Motor::ControlMethod_e &method, const PID &ppid,
             const PID &spid, bool use_kf):
  connect_(motor::connect_time_out_),
  ratio_(ratio),
  method_(method),
  ppid_(ppid),
  spid_(spid),
  use_kf_(use_kf),
  info_(){
    info_.type = type;
    info_.can_channel = 0;
    info_.id = 0;
    // 设定控制信号最大范围
    if (info_.type == Motor::M3508) {
      info_.max_intensity = 16384;
    } else if (info_.type == Motor::M2006) {
      info_.max_intensity = 10000;
    } else if (info_.type == Motor::GM6020) {
      info_.max_intensity = 30000;
    } else if (info_.type == Motor::MIT) {
      info_.max_intensity = 40;
    }
    mode_ = INIT;
    setFdbSrc(nullptr, nullptr);
    reset();
  }

void Motor::reset(void) {
  motor_data_.angle = 0;
  motor_data_.rotate_speed = 0;

  control_data_.target_angle = 0;
  control_data_.fdb_angle = 0;
  control_data_.target_speed = 0;
  control_data_.fdb_speed = 0;
  control_data_.feedforward_speed = 0;
  control_data_.target_torque = 0;
  control_data_.feedforward_intensity = 0;

  ppid_.reset();
  spid_.reset();
}

void Motor::rxCallback() {
  if (connect_.refresh() == CONNECT) {
      // 断连状态下收到反馈重置电机
      mode_ = INIT;
      reset();
  }
}
void Motor::CANIdConfig(const uint8_t &can_channel, const uint8_t &id) {
  info_.can_channel = can_channel;
  info_.id = id;
}

void Motor::setAngleSpeed(const float &target_angle, const float &ff_speed,
                          const float &ff_intensity) {
  control_data_.target_angle = target_angle;
  control_data_.feedforward_speed = ff_speed;
  control_data_.feedforward_intensity = ff_intensity;
}

void Motor::setFdbSrc(float *fdb_angle_src, float *fdb_speed_src) {
  if (fdb_angle_src != nullptr) {
      if (control_data_.fdb_angle_src != fdb_angle_src) {
        control_data_.fdb_angle_src = fdb_angle_src;
        control_data_.last_fdb_angle = *fdb_angle_src;
      }
  } else {
      // fdb_angle_src == nullptr
      if (control_data_.fdb_angle_src != &motor_data_.angle) {
        control_data_.fdb_angle_src = &motor_data_.angle;
        control_data_.last_fdb_angle = motor_data_.angle;
      }
  }
  if (fdb_speed_src != nullptr) {
      control_data_.fdb_speed_src = fdb_speed_src;
  } else {
      control_data_.fdb_speed_src = &motor_data_.rotate_speed;
  }
}

void Motor::resetFeedbackAngle(const float &angle) {
  control_data_.fdb_angle = angle;
}

void Motor::handle(void) {
  if (!connect_.check()){
      intensity_ = 0;
      motor_data_.rotate_speed = 0;
      return;
  }

  if (mode_ == INIT) { //初始化编码器角度记录（看起来好像是先上电初始化再断电
      motor_data_.last_ecd_angle = motor_data_.ecd_angle;
      control_data_.last_fdb_angle = *control_data_.fdb_angle_src;
      mode_ = POWEROFF;
      return;
  }
  //从反馈源更新反馈
  float fdb_angle_src = *control_data_.fdb_angle_src;
  control_data_.fdb_angle +=
      math::degNormalize180(fdb_angle_src - control_data_.last_fdb_angle);
  control_data_.fdb_speed = *control_data_.fdb_speed_src;
  control_data_.last_fdb_angle = fdb_angle_src;

  if (mode_ == WORKING){ //懒得写别的了，反正这次就一个速度单环
    if (method_ == SPEED){
      control_data_.target_angle = control_data_.fdb_angle;
      intensity_float_ =
          spid_.calc(control_data_.target_speed, control_data_.fdb_speed) +
          control_data_.feedforward_intensity;
    }
  } else

  if (mode_ == STOP){
    if (method_ == SPEED){
      control_data_.target_speed = 0;
      control_data_.feedforward_speed = 0;
      control_data_.feedforward_intensity = 0;
      intensity_float_ =
          spid_.calc(control_data_.target_speed, control_data_.fdb_speed);
    }
  }

  intensity_float_ = math::limit(intensity_float_ * math::sign(ratio_),
                                 -info_.max_intensity, info_.max_intensity);
  intensity_ = intensity_float_;

  //我猜这个负减速比是输入一个电流之后会向预期相反的方向转，srds为什么会有这种反人类的设计
}

void Motor::setAngle(const float &target_angle, const float &ff_intensity) {
  control_data_.target_angle = target_angle;
  control_data_.feedforward_intensity = ff_intensity;
}

void Motor::setSpeed(const float &target_speed, const float &ff_intensity) {
  control_data_.target_speed = target_speed;
  control_data_.feedforward_intensity = ff_intensity;
}
