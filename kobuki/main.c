#include <nrf_delay.h>

#include "helper_funcs.h"
#include "pixy.h"
#include "platform_kobuki.h"
#include "platform_bluetooth.h"
#include "Path_finding.h"
#include "Path_finding_2.h"
#include "Rotate.h"
#include "Trash_carrier.h"

Path_finding pf_fsm;
// Path_finding_2 pf_2_fsm;
Rotate r_fsm;
Trash_carrier tc_fsm;

void init_state_charts() {
  // initialize yakindu state machine
  // start statechart
  trash_carrier_init(&tc_fsm);
  trash_carrier_enter(&tc_fsm);

  path_finding_init(&pf_fsm);
  path_finding_enter(&pf_fsm);
  // path_finding_2_init(&pf_2_fsm);
  // path_finding_2_enter(&pf_2_fsm);
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
    // if(cycle_idx++ % 10 == 0)
    //   printf("Running %ld cycle\n", cycle_idx);
    cycle_idx++;

    trash_carrier_runCycle(&tc_fsm);
    printf("%d\n", is_bin_full());
    continue;

    if (false) {
      set_available(false);
      if (rotate_isActive(&r_fsm)) {
        rotate_runCycle(&r_fsm);
        if (rotate_isStateActive(&r_fsm, Rotate_main_region_Rest)) {
          rotate_exit(&r_fsm);
          // transit to the path finding
          lcd_printf(0, "bin is full");
          // TODO
        }
      }
    } else {
      rotate_runCycle(&r_fsm);
      if (cycle_idx % 10 == 0 && rotate_isStateActive(&r_fsm, Rotate_main_region_Rest)) {
        update_ultrasonic();
      }
    }

    // Delay before continuing
    // Note: removing this delay will make responses quicker
    // but will result in printf's in this loop breaking JTAG
    nrf_delay_ms(1);
  }
}

