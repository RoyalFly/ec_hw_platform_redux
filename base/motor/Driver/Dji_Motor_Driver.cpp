//
// Created by 98383 on 2023/11/21.
//

#include "Dji_Motor_Driver.h"
#include "can.h"
#include "..\..\base\common\math.h"

void DJIMotorDriver::idConfig(void) {
  for (int i=0;i<=10;i++){
    if (can1_motor_[i] != nullptr){
      can1_motor_[i]->CANIdConfig(1,i+1);
    }
  }
  for (int i=0;i<=10;i++){
    if (can2_motor_[i] != nullptr){
      can2_motor_[i]->CANIdConfig(2,i+1);
    }
  }
}

bool DJIMotorDriver::canRxMsgCheck(CAN_HandleTypeDef *hcan,
                                   CAN_RxHeaderTypeDef rx_header) {
  if (rx_header.StdId >= 0x201 && rx_header.StdId <=0x20b){
    return true;
  }else{
    return false;
  }
}

void DJIMotorDriver::canRxMsgCallback(CAN_HandleTypeDef *hcan,
                                      CAN_RxHeaderTypeDef rx_header,
                                      uint8_t *rx_data) {
  if (rx_header.StdId < 0x201 || rx_header.StdId > 0x20b){ //判断是不是Dji电调
    error_state_ = djimotor::ID_OUTRANGE;
    return;
  }

  Motor** can_motor;
  uint8_t id = rx_header.StdId - 0x200;
  djimotor::RawData_t* rawData;

  if (hcan == &hcan1){ //找到电机
    can_motor = can1_motor_;
    rawData = &can1_raw_data_[id-1];
  }else
  if (hcan == &hcan2){
    can_motor = can2_motor_;
    rawData = &can2_raw_data_[id-1];
  }

  rawData->ecd = (uint16_t) ((rx_data[0] << 8) || rx_data[1]); //解包
  rawData->rotate_speed_rpm = (uint16_t)((rx_data[2] << 8) || rx_data[3]);
  rawData->torq_current = (uint16_t) ((rx_data[4] << 8) || rx_data[5]);
  rawData->temp = rx_data[6];

  if (can_motor[id-1] == nullptr){ //未定义电机
    error_state_ = djimotor::MOTOR_UNDEFINED;
    return;
  }

  Motor* m = can_motor[id-1];

  if (m->info_.type == Motor::GM6020 || m->info_.type == Motor::M3508 || //是dji电机就更新
      m->info_.type == Motor::M2006){
    m->motor_data_.ecd_angle = math::ecd2deg(rawData->ecd, 8192);

    float delta = m->motor_data_.ecd_angle - m->motor_data_.last_ecd_angle;
    delta = math::degNormalize180(delta) / m->ratio_;
    m->motor_data_.angle += delta;

    m->motor_data_.rotate_speed = math::rpm2dps(rawData->rotate_speed_rpm) / m->ratio_;

    m->motor_data_.temp = rawData->temp;
    m->motor_data_.current = rawData->torq_current;
    m->motor_data_.last_ecd_angle = m->motor_data_.ecd_angle;
  }else{
    error_state_ = djimotor::TYPE_MISMATCH;
  }

  m->rxCallback(); //刷新一下连接状态
  error_state_ = djimotor::NO_ERROR;
}

void DJIMotorDriver::canTxMsg(uint8_t can_channel, //发包
                              djimotor::CANIDRange_e id_range) {
  Motor** can_motor;
  if (can_channel == 1) {
    hcan_ = &hcan1;
    can_motor = can1_motor_;
  } else
  if (can_channel == 2) {
    hcan_ = &hcan2;
    can_motor = can2_motor_;
  } else {
    return;
  }

  // 报文头
  can_tx_header_.IDE = CAN_ID_STD;
  can_tx_header_.RTR = CAN_RTR_DATA;
  can_tx_header_.DLC = 8;
  if (id_range == djimotor::ID_1_4) {
    can_tx_header_.StdId = 0x200;
    motor_id_base_ = 0;
  } else if (id_range == djimotor::ID_5_8) {
    can_tx_header_.StdId = 0x1ff;
    motor_id_base_ = 4;
  } else if (id_range == djimotor::ID_9_11) {
    can_tx_header_.StdId = 0x2ff;
    motor_id_base_ = 8;
  }
  for (int i = 0; i < 4; i++) {
    if (can_motor[i + motor_id_base_] == nullptr) {
      can_tx_data_[2 * i] = 0;
      can_tx_data_[2 * i + 1] = 0;
    } else {
      can_tx_data_[2 * i] = (can_motor[i + motor_id_base_]->intensity_ >> 8);
      can_tx_data_[2 * i + 1] = (can_motor[i + motor_id_base_]->intensity_);
    }
  }

  HAL_CAN_AddTxMessage(hcan_, &can_tx_header_, can_tx_data_, &can_tx_mail_box_);
}
