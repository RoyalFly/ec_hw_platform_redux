//
// Created by 98383 on 2023/11/20.
//

#include "filter.h"

float LowPassFilter::update(float input) {
  input_ = input;
  output_ = output_ * (1 - k_) + input_ * k_;
  return output_;
}
