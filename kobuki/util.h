#ifndef _UTIL_H_
#define _UTIL_H_



#include "buckler.h"
#include "display.h"
#include "kobukiActuator.h"
#include "kobukiSensorPoll.h"
#include "kobukiSensorTypes.h"
#include "kobukiUtilities.h"
#include "mpu9250.h"
#include "simple_ble.h"

void init_robot(void);
void display_float(float x);
void display_string(const char *str);
float get_rotate(void);
float set_full(bool full);

#endif