//
// Created by 98383 on 2023/11/12.
//

#include "../base/Remote/Remote.h"
#include "robot.h"
#include <cstring>
#include "../base/motor/motor.h"
#include "stm32f4xx_hal_can.h"
#include "..\base\motor\Driver\Dji_Motor_Driver.h"

extern RC rc;
extern uint8_t rx_buff_[100];
extern uint8_t flagg;
int rx_cnt = 0;

const PID chassis_wheel_spid(40, 1, 10, 1000, 16384);
Motor User_Motor(Motor::M3508, 3591.f / 187.f, Motor::SPEED,  // type, ratio, method
           PID(), PID(chassis_wheel_spid));             // ppid, spid

Motor* can1_dji_motor[11] = {
    &User_Motor,    // id:1
    nullptr,    // id:2
    nullptr,    // id:3
    nullptr,    // id:4
    nullptr,  // id:5
    nullptr,  // id:6
    nullptr,  // id:7
    nullptr,  // id:8
    nullptr,  // id:9
    nullptr,  // id:10
    nullptr   // id:11
};
Motor* can2_dji_motor[11] = {
    nullptr,  // id:1
    nullptr,     // id:2
    nullptr,     // id:3
    nullptr,     // id:4
    nullptr,  // id:5
    nullptr,  // id:6
    nullptr,  // id:7
    nullptr,  // id:8
    nullptr,  // id:9
    nullptr,  // id:10
    nullptr   // id:11
};
DJIMotorDriver dji_motor_driver(can1_dji_motor, can2_dji_motor);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM6) {
    if (flagg == 0){
      MainControlinit();
      flagg = 1;
    }
    MainControlloop();
  }
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
//  if (rc.uartCheck(huart)){
//    rc.rxCallback();
//  }
//}

//
//void User_UART_IdleHandler(UART_HandleTypeDef* huart) {
//  // Judge if idle enabled. 判断空闲中断是否使能
//  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
//    // Clear idle flag. 清除空闲中断标记
//    __HAL_UART_CLEAR_IDLEFLAG(huart);
//    // idle中断回调
//    if (rc.uartCheck(huart)) {
//      rc.idleCallback();
//    }
//  }
//}
//
//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,uint16_t Size){
//  if (huart->Instance == USART3){
//   // if (Size > RC_FRAME_LEN){
//	rc.rx_len_ = Size;
//        rx_cnt++;
//        HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,GPIO_PIN_SET);
//        rc.connect_.refresh();
//        memcpy(rc.rx_data_, rx_buff_, rc.rx_len_);
//        HAL_UARTEx_ReceiveToIdle_DMA(&huart3,rx_buff_,100);
//   // }
//
//  }
//}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  if (huart->Instance == USART3){
  //HAL_GPIO_TogglePin(GPIOG,GPIO_PIN_1);
        rc.connect_.refresh();
        rx_cnt++;
        memcpy(rc.rx_data_, rx_buff_, 18);
        HAL_UART_Receive_IT(huart,rx_buff_,18);
  }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
  CAN_RxHeaderTypeDef rx_header;
  uint8_t data[8];
  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, data);

  if (dji_motor_driver.canRxMsgCheck(hcan, rx_header)){
    dji_motor_driver.canRxMsgCallback(hcan, rx_header, data);
  }
}
