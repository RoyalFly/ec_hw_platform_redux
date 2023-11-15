//
// Created by 98383 on 2023/11/12.
//

#ifndef EC_HW_PLATFORM_REMOTE_H
#define EC_HW_PLATFORM_REMOTE_H

#include "../common/Connect.h"
#include "usart.h"
#include <cstdint>

#define RC_RX_BUF_SIZE 36u
#define RC_FRAME_LEN 18u

typedef uint16_t RCKey;

class RC{
  public:
    typedef enum RCswitchstate{
      UP,
      MID,
      DOWN,
    } RCswitchstate_e;
  public:
    RC(UART_HandleTypeDef* huart = nullptr);
    void init(void);
    void handle(void);
    void reset(void);
    void rxCallback(void);
    void idleCallback(uint16_t Size);
    bool uartCheck(UART_HandleTypeDef* huart) { return huart == huart_; }

  public:
    Connect connect_;
    //遥控器通道
    struct RC_Channel{
      int16_t r_row;
      int16_t r_col;
      int16_t l_row;
      int16_t l_col;
      int16_t dial_wheel;
    } channel_;
    //遥控器拨挡
    struct RCswitch{
      RCswitchstate l;
      RCswitchstate r;
    } switch_;
    RCKey key_;
    uint16_t rx_len_;

  public:
    UART_HandleTypeDef* huart_;
    uint8_t rx_buf_[100], rx_data_[100];

    struct RCRaw {
      int16_t ch[5];
      uint8_t s[2];
    } rc_raw_;
};

#endif // EC_HW_PLATFORM_REMOTE_H
