//
// Created by 98383 on 2023/11/12.
//

#ifndef EC_HW_PLATFORM_HARDWARE_CONFIG_H
#define EC_HW_PLATFORM_HARDWARE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// 开发板型号
#define DBC

// 遥控器串口
#define RC_UART &huart3
// 遥控器型号
//#define RF206S
#define DR16
// 视觉通信串口
#define CV_UART &huart1
// 裁判系统通信串口
#define REFEREE_UART &huart6
// 调试串口
// #define DEBUG_UART &huart1
#define DEBUG_USB

// imu相关(spi/i2c/tim)
#define BMI088_SPI &hspi1
#define IST8310_I2C &hi2c3
#define BOARD_IMU_HEAT_TIM &htim10
#define BOARD_IMU_HEAT_CHANNEL TIM_CHANNEL_1

#ifdef __cplusplus
}
#endif

#endif // EC_HW_PLATFORM_HARDWARE_CONFIG_H
