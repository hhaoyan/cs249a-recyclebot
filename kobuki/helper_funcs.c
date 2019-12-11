#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <buckler.h>
#include <mpu9250.h>

#include "helper_funcs.h"
#include "platform_kobuki.h"

float normalize_rot(float desired, float now) {
  desired -= floor(desired / 360.0f) * 360.0f;
  now -= floor(now / 360.0f) * 360.0f;
  // Never cross 180 deg
  if(desired < 180 && now > 180) {
    return 360 - (now - desired);
  } else if (desired > 180 && now < 180) {
    return -360 + (desired - now);
  } else {
    return desired - now;
  }
}

float update_dist(float dist, uint16_t prev_encoder, bool is_forward){
  const float CONVERSION = 0.00008529;
  uint16_t current_encoder = read_encoder();
  float result = 0.0;
  if (!is_forward){
    uint16_t temp = current_encoder;
    current_encoder = prev_encoder;
    prev_encoder = temp;
  }
  if (current_encoder >= prev_encoder) {
    // normal case
    result = (float)current_encoder - (float)prev_encoder;
  } else {
    // account for wrap
    result = (float)current_encoder + (0xFFFF - (float)prev_encoder);
  }
  result = result * CONVERSION;
  if (result> 1.0 || result< -1.0){
    return dist;
  }else{
    return dist +result;
  }
}

float read_tilt_theta(void){
  return asin(mpu9250_read_accelerometer().x_axis)/M_PI*180;
}

float read_tilt_psi(void){
  return asin(mpu9250_read_accelerometer().y_axis)/M_PI*180;
}

float get_abs(float var){
  return fabs(var);
}
