//
// Created by 98383 on 2023/11/12.
//

#include "Connect.h"
#include "main.h"

bool Connect::check(void){
  if (HAL_GetTick() - last_tick_ > timeout_){
    if (flag_){
      edge_ = DISCONNECT;
      freq_ = 0;
    } else {
      edge_ = UNCHANGED;
    }
    flag_ = false;
  } else {
    if (flag_){
      edge_ = UNCHANGED;
    }
  }
  return flag_;
}

ConnectEdge Connect::refresh() {
  if (!flag_) {
    edge_ = CONNECT;
    flag_ = true;
  } else {
    edge_ = UNCHANGED;
  }
  if (HAL_GetTick() - last_tick_ < 1) {
    freq_ = 1e3f;
  } else {
    freq_ = 1e3f / (HAL_GetTick() - last_tick_);
  }
  last_tick_ = HAL_GetTick();
  return edge_;
}
