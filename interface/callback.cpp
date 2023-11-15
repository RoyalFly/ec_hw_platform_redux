//
// Created by 98383 on 2023/11/12.
//

#include "../base/Remote/Remote.h"
#include "robot.h"
#include "main.h"
#include <cstring>
//#include "stm32f407xx.h"
//#include "stm32f4xx_hal_uart.h"
//#include "stm32f4xx_it.h"
//#include "..\hardware_config.h"

extern RC rc;
extern uint8_t rx_buff_[100];
extern uint8_t flagg;
extern DMA_HandleTypeDef hdma_usart3_rx;

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

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,uint16_t Size){
  if (huart->Instance == USART3){
   // if (Size > RC_FRAME_LEN){
	rc.rx_len_ = sizeof(rx_buff_) - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
        HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,GPIO_PIN_SET);
        rc.connect_.refresh();
        memcpy(rc.rx_data_, rx_buff_, rc.rx_len_);
        memset(rx_buff_, 0, rc.rx_len_);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3,rx_buff_,sizeof(rx_buff_));
   // }

  }
}
