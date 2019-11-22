#include "util.h"
#include "state.h"
#include "pixy.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "app_timer.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_spi.h"

#include "buckler.h"
#include "display.h"
#include "kobukiActuator.h"
#include "kobukiSensorPoll.h"
#include "kobukiSensorTypes.h"
#include "kobukiUtilities.h"
#include "mpu9250.h"

#include "states.h"
#include "helper_funcs.h"
#include "Robot_template.h"

// global variables
KobukiSensors_t sensors = {0};

int main(void) {
  ret_code_t error_code = NRF_SUCCESS;

  init_robot();
  // initialize yakindu state machine
  // start statechart
  Robot_template sc_handle;
  robot_template_init(&sc_handle);
  robot_template_enter(&sc_handle);

  // intialize statechart variables
  // if needed

  pixy_init();

  uint16_t width, height;
  pixy_get_resolution(&width, &height);
  pixy_set_lamp(1, 0);

  uint8_t x0, y0, x1, y1;

  // loop forever, running state machine
  while (1) {
    // read sensors from robot
    kobukiSensorPoll(&sensors);

    // delay before continuing
    // Note: removing this delay will make responses quicker, but will result
    //  in printf's in this loop breaking JTAG
    nrf_delay_ms(1);

    // iterate statechart
    robot_template_runCycle(&sc_handle);
  }
}

