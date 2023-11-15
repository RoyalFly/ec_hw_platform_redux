//
// Created by 98383 on 2023/11/12.
//

#include "Remote.h"
#include <string.h>
#include "stm32f4xx_hal_uart.h"

// max time to wait for connect(ms)
const uint32_t rc_connect_timeout = 100;

const int16_t rc_ch_offset = 1024;
const int16_t rc_ch_min = 364;   // -660
const int16_t rc_ch_max = 1684;  // 660

extern uint8_t rx_buff_[100];

RC::RC(UART_HandleTypeDef* huart) : huart_(huart),connect_(rc_connect_timeout){
  rx_len_ = 0;
  switch_.l = DOWN;
  switch_.r = DOWN;
}

void RC::init(void){
  rx_len_ = 0;
  reset();
//  if (huart_ != nullptr){
//    __HAL_UART_ENABLE_IT(huart_, UART_IT_IDLE);
//    HAL_UART_Receive_DMA(huart_,rx_buf_,1);
//  }
}

void RC::reset(void) {
  rc_raw_.ch[0] = 0;
  rc_raw_.ch[1] = 0;
  rc_raw_.ch[2] = 0;
  rc_raw_.ch[3] = 0;
  rc_raw_.s[0] = 0;
  rc_raw_.s[1] = 0;
  key_ = 0;
  rc_raw_.ch[4] = 0;

  channel_.r_row = 0;
  channel_.r_col = 0;
  channel_.l_row = 0;
  channel_.l_col = 0;
  channel_.dial_wheel = 0;
}

void RC::handle(void) {
  // Check connection
  if (!connect_.check()) {
    HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,GPIO_PIN_SET);
    reset();
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3,rx_buff_,sizeof(rx_buff_));
    return;
  }

  // Unpack data. 数据解包
  rc_raw_.ch[0] = (rx_data_[0] | rx_data_[1] << 8) & 0x07ff;       // Channel 0
  rc_raw_.ch[1] = (rx_data_[1] >> 3 | rx_data_[2] << 5) & 0x07ff;  // Channel 1
  rc_raw_.ch[2] = (rx_data_[2] >> 6 | rx_data_[3] << 2 | rx_data_[4] << 10) &
                  0x07ff;                                          // Channel 2
  rc_raw_.ch[3] = (rx_data_[4] >> 1 | rx_data_[5] << 7) & 0x07ff;  // Channel 3
  rc_raw_.s[0] = (rx_data_[5] >> 4 & 0x0003);        // Switch left
  rc_raw_.s[1] = (rx_data_[5] >> 4 & 0x000C) >> 2;   // Switch right
  key_ = rx_data_[14] | rx_data_[15] << 8;           // Keyboard
  rc_raw_.ch[4] = rx_data_[16] | rx_data_[17] << 8;  // Dial wheel

  channel_.r_row = rc_raw_.ch[0] - rc_ch_offset;
  channel_.r_col = rc_raw_.ch[1] - rc_ch_offset;
  channel_.l_row = rc_raw_.ch[2] - rc_ch_offset;
  channel_.l_col = rc_raw_.ch[3] - rc_ch_offset;
  channel_.dial_wheel = rc_raw_.ch[4] - rc_ch_offset;
  if (rc_raw_.s[0] == 1) {
    switch_.r = UP;
  } else if (rc_raw_.s[0] == 2) {
    switch_.r = DOWN;
  } else if (rc_raw_.s[0] == 3) {
    switch_.r = MID;
  }
  if (rc_raw_.s[1] == 1) {
    switch_.l = UP;
  } else if (rc_raw_.s[1] == 2) {
    switch_.l = DOWN;
  } else if (rc_raw_.s[1] == 3) {
    switch_.l = MID;
  }
}

void RC::idleCallback(uint16_t Size) {
  //if (Size >= RC_FRAME_LEN) {
    connect_.refresh();
    memcpy(rx_data_, rx_buff_, Size);
    memset(rx_buff_, 0, Size);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3,rx_buff_,sizeof(rx_buff_));
  //}
}
