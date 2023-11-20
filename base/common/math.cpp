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