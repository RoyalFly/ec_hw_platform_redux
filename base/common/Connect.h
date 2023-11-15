//
// Created by 98383 on 2023/11/12.
//

#ifndef EC_HW_PLATFORM_CONNECT_H
#define EC_HW_PLATFORM_CONNECT_H

#include "stdint.h"

typedef enum ConnectEdge{
  UNCHANGED,
  CONNECT,
  DISCONNECT,
} ConnectEdge_e;

class Connect{
  public:
    Connect(uint32_t timeout): flag_(false), last_tick_(0), timeout_(timeout), freq_(0) {}
    bool check(void);
    ConnectEdge refresh(void);
    float freq(void) { return freq_; }
  private:
    bool flag_;
    ConnectEdge edge_;
    uint32_t last_tick_;
    uint32_t timeout_;
    float freq_;
};

#endif // EC_HW_PLATFORM_CONNECT_H
