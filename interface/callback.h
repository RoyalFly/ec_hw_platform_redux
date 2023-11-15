//
// Created by 98383 on 2023/11/12.
//

#ifndef EC_HW_PLATFORM_CALLBACK_H
#define EC_HW_PLATFORM_CALLBACK_H

#include "usart.h"

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart);
void User_UART_IdleHandler(UART_HandleTypeDef* huart);

#endif // EC_HW_PLATFORM_CALLBACK_H
