#include <nrf_delay.h>

#include "helper_funcs.h"
#include "pixy.h"
#include "platform_kobuki.h"
#include "platform_bluetooth.h"
#include "Path_finding.h"
#include "Rotate.h"
#include "states.h"

Path_finding pf_fsm;
Rotate r_fsm;

void init_state_charts() {
  // initialize yakindu state machine
  // start statechart
  path_finding_init(&pf_fsm);
  path_finding_enter(&pf_fsm);
  rotate_init(&r_fsm);
  rotate_enter(&r_fsm);
}

int main(void) {
  // intialize platform
  init_bluetooth();
  // Important! Bluetooth must be initialized before
  // kobuki. I don't understand why. -Haoyan
  init_kobuki();
  init_state_charts();

  uint16_t width, height;
  pixy_init();
  pixy_get_resolution(&width, &height);
  pixy_set_lamp(0, 0);

  //uint8_t x0, y0, x1, y1;

  // loop forever, running state machine
  uint32_t cycle_idx;
  while (1) {
    update_sensors();
    printf("Running %d cycle\n", cycle_idx++);

    // iterate statechart
    rotate_runCycle(&r_fsm);
    // path_finding_runCycle(&pf_fsm);

    // Delay before continuing
    // Note: removing this delay will make responses quicker
    // but will result in printf's in this loop breaking JTAG
    nrf_delay_ms(1);
  }
}

