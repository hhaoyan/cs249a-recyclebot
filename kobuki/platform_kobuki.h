#ifndef _PLATFORM_KOBUKI_H_
#define _PLATFORM_KOBUKI_H_

#include <stdint.h>
#include <kobukiSensorTypes.h>

typedef enum {
	LCD_LINE_0 = 0,
	LCD_LINE_1 = 1,
} lcd_line_t;

extern KobukiSensors_t sensors;
extern bool pixy_line_detected;
extern uint8_t pixy_line_start[2], pixy_line_end[2];

extern void init_kobuki();
extern void update_sensors();

extern bool is_left_cliff();
extern bool is_center_cliff();
extern bool is_right_cliff();
extern bool is_center_bumper();
extern bool is_left_bumper();
extern bool is_right_bumper();

inline bool is_vec_detected(){return pixy_line_detected;}
inline uint8_t vec_start_x(){return pixy_line_start[0];}
inline uint8_t vec_start_y(){return pixy_line_start[1];}
inline uint8_t vec_end_x(){return pixy_line_end[0];}
inline uint8_t vec_end_y(){return pixy_line_end[1];}
extern bool is_button_press();

extern void start_gyro();
extern float read_gyro();
extern void stop_gyro();

extern uint16_t read_encoder();

extern void drive_kobuki(int16_t left_wheel, int16_t right_wheel);
extern void stop_kobuki();

extern int lcd_printf(lcd_line_t line, const char* fmt, ...)
__attribute__ ((format (printf, 2, 3)));
extern void lcd_clear();

extern bool is_ultrasonic_full();
extern void update_ultrasonic();
extern bool is_full();
extern void clear_full();

#endif
