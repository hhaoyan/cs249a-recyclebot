#include <kobukiSensorTypes.h>
#include <nrf_delay.h>

#include "helper_funcs.h"
#include "pixy.h"
#include "platform_kobuki.h"
#include "platform_bluetooth.h"
#include "Robot_template.h"
#include "states.h"

int main(void) {
  // intialize statechart variables
  // if needed
  init_kobuki();
  init_bluetooth();
  pixy_init();

  // initialize yakindu state machine
  // start statechart
  Robot_template sc_handle;
  robot_template_init(&sc_handle);
  robot_template_enter(&sc_handle);

  uint16_t width, height;
  pixy_get_resolution(&width, &height);
  pixy_set_lamp(1, 0);

  //uint8_t x0, y0, x1, y1;

  // loop forever, running state machine
  while (1) {
    // Delay before continuing
    // Note: removing this delay will make responses quicker, but will result
    //  in printf's in this loop breaking JTAG
    nrf_delay_ms(1);

    // iterate statechart
    robot_template_runCycle(&sc_handle);
  }
}

