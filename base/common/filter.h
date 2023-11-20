//
// Created by 98383 on 2023/11/20.
//

#ifndef EC_HW_PLATFORM_REDUX_FILTER_H
#define EC_HW_PLATFORM_REDUX_FILTER_H

class LowPassFilter {
public:
  LowPassFilter(void) : LowPassFilter(1.f, 0) {}
  LowPassFilter(float k) : LowPassFilter(k, 0) {}
  LowPassFilter(float k, float init) : k_(k), output_(init) {}

  float update(float input);
  inline void setK(float k) { k_ = k; }
  inline void reset(float val = 0) {
    input_ = val;
    output_ = val;
  }

private:
  float k_;
  float input_, output_;
};

#endif // EC_HW_PLATFORM_REDUX_FILTER_H
