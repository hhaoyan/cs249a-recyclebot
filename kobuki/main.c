#include <nrf_delay.h>

#include "helper_funcs.h"
#include "pixy.h"
#include "platform_kobuki.h"
#include "platform_bluetooth.h"
#include "Path_finding.h"
#include "Path_finding_2.h"
#include "Rotate.h"

Path_finding pf_fsm;
Path_finding_2 pf_2_fsm;
Rotate r_fsm;

void init_state_charts() {
  // initialize yakindu state machine
  // start statechart
  path_finding_init(&pf_fsm);
  path_finding_enter(&pf_fsm);
  path_finding_2_init(&pf_2_fsm);
  path_finding_2_enter(&pf_2_fsm);
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
  pixy_init();

  // Turn on the lamp
  pixy_set_lamp(1, 0);

  // loop forever, running state machine
  uint32_t cycle_idx = 0;
  while (1) {
    update_sensors();
    if(cycle_idx++ % 10 == 0)
      printf("Running %ld cycle\n", cycle_idx);

    // iterate statechart
    // rotate_runCycle(&r_fsm);
    path_finding_runCycle(&pf_fsm);
    // path_finding_2_runCycle(&pf_2_fsm);

    // Delay before continuing
    // Note: removing this delay will make responses quicker
    // but will result in printf's in this loop breaking JTAG
    nrf_delay_ms(1);
  }
}

