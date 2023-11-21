//
// Created by 98383 on 2023/11/12.
//

#include "../base/Remote/Remote.h"
#include "robot.h"
#include <cstring>
#include "../base/motor/motor.h"
//#include "stm32f407xx.h"
#include "stm32f4xx_hal_can.h"
//#include "stm32f4xx_it.h"
//#include "..\hardware_config.h"

extern RC rc;
extern uint8_t rx_buff_[100];
extern uint8_t flagg;
extern DMA_HandleTypeDef hdma_usart3_rx;
int rx_cnt = 0;

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

  if (rx_header.StdId >= 0x201 && rx_header.StdId <= 0x20b){

  }
}
