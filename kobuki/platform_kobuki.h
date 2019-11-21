#ifndef _PLATFORM_KOBUKI_H_
#define _PLATFORM_KOBUKI_H_

typedef enum {
	LCD_LINE_0 = 0,
	LCD_LINE_1 = 1,
} lcd_line_t;

extern void init_kobuki(void);
extern int lcd_printf(lcd_line_t line, const char* fmt, ...)
__attribute__ ((format (printf, 2, 3)));

#endif
