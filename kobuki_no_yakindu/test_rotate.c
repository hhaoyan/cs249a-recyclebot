#include <stdio.h>
#include <stdarg.h>
#include "src-gen/Rotate.h"

float rotate = 0.0;
float get_rotate(){
	return rotate;
}

int dir = 0;
void drive_kobuki(int16_t left_wheel, int16_t right_wheel){
	printf("%d %d\n", left_wheel, right_wheel);
	dir = left_wheel > right_wheel ? -1 : 1;
}

int lcd_printf(lcd_line_t line, const char* fmt, ...){
	va_list valist;
	va_start(valist, fmt);
	vprintf(fmt, valist);
	printf("\n");
	return 0;
}

float normalize_rot(float x) {
  int n = round(x / 360.0f);
  return x - n * 360.0f;
}

float gyro = 0.0f;

float read_gyro() {
	gyro += 0.5 * dir;
	return gyro;
}

void start_gyro(){
	gyro = 0.0f;
}

void stop_gyro(){}

void stop_kobuki(){}

int main(){
	Rotate r_fsm;
	rotate_init(&r_fsm);
  	rotate_enter(&r_fsm);

  	int cycle = 0;
  	while (1) {
  		if(cycle == 100){
  			rotate = -100;
  		}
    	rotate_runCycle(&r_fsm);
    	cycle ++;
    	printf("Cycle %d\n", cycle);
  	}
	return 0;
}