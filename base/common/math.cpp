#include "math.h"

float math::limit(float val, const float& min, const float& max) {
  if (min > max)
    return val;
  else if (val < min)
    val = min;
  else if (val > max)
    val = max;
  return val;
}

float math::rad2deg(const float &angle) {
  return angle / (2 * 3.14159265358f) * 360.0f;
}

float math::deg2rad(const float &angle) {
  return angle / 360.0f * (2 * 3.14159265358f);
}

float math::ecd2deg(const float &ecd, const float &ecd_range) {
  return (ecd / ecd_range) * 360.0f;
}

float math::ecd2rad(const float &ecd, const float &ecd_range) {
  return (ecd / ecd_range) * (2 * 3.14159265358f);
}

float math::loopLimit(float val, const float& min, const float& max) {
  if (min >= max)
    return val;
  if (val > max) {
    while (val > max)
      val -= (max - min);
  } else if (val < min) {
    while (val < min)
      val += (max - min);
  }
  return val;
}

float math::degNormalize180(const float &angle) {
  return math::loopLimit(angle, -180.0f, 180.0f);
}

float math::rpm2dps(const float &w) {
  return w / 60.f * 2 * 3.14159265358f;
}

float math::sign(const float& val){
  if (val > 0){
    return 1;
  } else
  if (val < 0){
    return -1;
  } else {
    return 0;
  }
}
