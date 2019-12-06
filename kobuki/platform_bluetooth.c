#include <simple_ble.h>

#include "platform_bluetooth.h"

void ble_evt_write(ble_evt_t const* p_ble_evt) {
    // TODO: logic for each characteristic and related state changes
}

// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
        // c0:98:e5:49:xx:xx
        .platform_id       = 0x49,    // used as 4th octect in device BLE address
        .device_id         = 0x2048, // TODO: replace with your lab bench number
        .adv_name          = "recyclebot", // used in advertisements if there is room
        .adv_interval      = MSEC_TO_UNITS(1000, UNIT_0_625_MS),
        .min_conn_interval = MSEC_TO_UNITS(100, UNIT_1_25_MS),
        .max_conn_interval = MSEC_TO_UNITS(200, UNIT_1_25_MS),
};

//4607eda0-f65e-4d59-a9ff-84420d87a4ca
static simple_ble_service_t robot_service = {{
    .uuid128 = {0xca,0xa4,0x87,0x0d,0x42,0x84,0xff,0xA9,
                0x59,0x4D,0x5e,0xf6,0xa0,0xed,0x07,0x46}
}};

static int rot = 0;
static bool available = true;
static simple_ble_char_t rot_state_char = {.uuid16 = 0x108a};
static simple_ble_char_t avl_state_char = {.uuid16 = 0x108b};
static simple_ble_app_t* simple_ble_app;

void init_bluetooth() {
  // Setup BLE
  simple_ble_app = simple_ble_init(&ble_config);

  simple_ble_add_service(&robot_service);
  simple_ble_add_characteristic(1, 1, 0, 0,
      sizeof(rot), (uint8_t*)&rot,
      &robot_service, &rot_state_char);
  simple_ble_add_characteristic(1, 1, 0, 0,
      sizeof(available), (uint8_t*)&available,
      &robot_service, &avl_state_char);

  // Start Advertising
  simple_ble_adv_only_name();
}

float get_rotate() {
  return rot;
}

void set_available(bool avl) {
  available = avl;
  if (!avl) {
    rot = 0;
  }
}
