#include <nrf_delay.h>

#include "helper_funcs.h"
#include "pixy.h"
#include "platform_kobuki.h"
#include "platform_bluetooth.h"
#include "Trash_carrier.h"

int main(void) {
  // intialize platform
  init_bluetooth();
  // Important! Bluetooth must be initialized before
  // kobuki. I don't understand why. -Haoyan
  init_kobuki();
  pixy_init();

  Trash_carrier tc_fsm;
  trash_carrier_init(&tc_fsm);
  trash_carrier_enter(&tc_fsm);

  // Turn on the lamp
  pixy_set_lamp(1, 1);

  // loop forever, running state machine
  while (1) {
    update_sensors();

    trash_carrier_runCycle(&tc_fsm);

    // Delay before continuing
    // Note: removing this delay will make responses quicker
    // but will result in printf's in this loop breaking JTAG
    nrf_delay_ms(1);
  }
}

