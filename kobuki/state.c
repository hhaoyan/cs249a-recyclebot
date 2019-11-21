#include <math.h>
#include <stddef.h>

#include <kobukiActuator.h>
#include <kobukiSensorPoll.h>
#include <kobukiSensorTypes.h>
#include <mpu9250.h>
#include <nrf_delay.h>

#include "state.h"
#include "platform_kobuki.h"
#include "platform_bluetooth.h"

KobukiSensors_t sensors = {0};
static state_t state = Work;
static float rotate = 0.0f;
static float rot_diff = 0.0f;

static float normalize_rot(float x) {
  int n = round(x / 360.0f);
  return x - n * 360.0f;
}

static void work() {
  lcd_printf(LCD_LINE_0, "work");

  float new_rot = get_rotate();
  float diff = normalize_rot(new_rot - rotate);
  if (fabs(diff) > 5.0f) {
    rot_diff = diff;
    state = Rotating;
  }
  kobukiDriveDirect(0, 0);
}

static void rot_entry() {
  mpu9250_start_gyro_integration();
}

static void rot_leave() {
  mpu9250_stop_gyro_integration();
  kobukiDriveDirect(0, 0);
}

static void rot() {
  lcd_printf(LCD_LINE_0, "Rotate");

  float rot_now = normalize_rot(mpu9250_read_gyro_integration().z_axis);
  lcd_printf(LCD_LINE_1, "%f", rot_now);

  if (fabs(rot_now) > fabs(rot_diff)) {
    rotate = normalize_rot(rotate + rot_now);
    state = Work;
  } else if (rot_diff > 0) {
    kobukiDriveDirect(-150, 150);
  } else {
    kobukiDriveDirect(150, -150);
  }
}

typedef void (*fsm_callback)(void);

typedef struct {
  fsm_callback update;
  fsm_callback entry;
  fsm_callback leave;
} fsm_t;

static fsm_t fsm[Tail] = {
  { work, NULL, NULL },
  { rot, rot_entry, rot_leave},
};

static void callback(fsm_callback cb) {
  if (cb) {
    cb();
  }
}

void fsm_loop(void) {
  state_t last = Tail;
  while (true) {
    kobukiSensorPoll(&sensors);
    if (last != state) {
      callback(fsm[state].entry);
    }
    last = state;
    callback(fsm[last].update);
    if (last != state) {
      callback(fsm[last].leave);
    }
    nrf_delay_ms(1);
  }
}
