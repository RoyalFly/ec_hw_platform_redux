//
// Created by 98383 on 2023/11/20.
//

#ifndef EC_HW_PLATFORM_REDUX_PID_H
#define EC_HW_PLATFORM_REDUX_PID_H

#include "filter.h"

class PID {
public:
  PID(void) : PID(0, 0, 0, 0, 0) {}
  PID(float kp, float ki, float kd, float i_max, float out_max,
      float d_filter_k = 1);

  void reset(void);
  float calc(float ref, float fdb);

public:
  float kp_, ki_, kd_;
  float i_max_, out_max_;
  float output_;
  LowPassFilter d_filter_;

protected:
  float ref_, fdb_;
  float err_, err_sum_, last_err_;
  float pout_, iout_, dout_;
};

#endif // EC_HW_PLATFORM_REDUX_PID_H
