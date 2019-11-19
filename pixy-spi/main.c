#include <stdio.h>
#include <stdlib.h>

#include "display.h"
#include "pixy.h"
#include "buckler.h"

static nrf_drv_spi_t spi_instance = NRF_DRV_SPI_INSTANCE(1);

void display_string(const char *str) {
  static char buf[16];
  snprintf(buf, 16, "%s", str);
  display_write(buf, DISPLAY_LINE_0);
}

void init_spi_display(){
  // initialize display
  nrf_drv_spi_config_t spi_config = {
    .sck_pin = BUCKLER_LCD_SCLK,
    .mosi_pin = BUCKLER_LCD_MOSI,
    .miso_pin = BUCKLER_LCD_MISO,
    .ss_pin = BUCKLER_LCD_CS,
    .irq_priority = NRFX_SPI_DEFAULT_CONFIG_IRQ_PRIORITY,
    .orc = 0,
    .frequency = NRF_DRV_SPI_FREQ_4M,
    .mode = NRF_DRV_SPI_MODE_2,
    .bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST
  };
  ret_code_t error_code = nrf_drv_spi_init(&spi_instance, &spi_config, NULL, NULL);
  APP_ERROR_CHECK(error_code);
  display_init(&spi_instance);
  display_write("hell", DISPLAY_LINE_0);
  printf("Display initialized!\n");

  display_string("Hello!");
}

int main(void) {
  pixy_init();

  init_spi_display();
  printf("Started\n");

  uint16_t width, height;
  pixy_get_resolution(&width, &height);
  pixy_set_lamp(1, 0);

  uint8_t x0, y0, x1, y1;
  static char buf[16];
  // barcode* barcodes;
  while(1){
    if(pixy_get_line_vector(&x0, &y0, &x1, &y1)){
      snprintf(buf, 16, "No vector!");
      display_write(buf, DISPLAY_LINE_0);
      snprintf(buf, 16, "                ");
      display_write(buf, DISPLAY_LINE_1);
    }else{
      snprintf(buf, 16, "Vector detected!");
      display_write(buf, DISPLAY_LINE_0);
      snprintf(buf, 16, "%d,%d-%d,%d", x0, y0, x1, y1);
      display_write(buf, DISPLAY_LINE_1);
    }

    // get_pixy_vector();
    
    /*printf("*******Request bar codes*******\n");

    int n_codes = pixy_get_barcodes(&barcodes);

    if(n_codes > 0){
      for(int i=0;i<n_codes;i++){
        printf("Code type %d, length %d, data: %s\n", 
          barcodes[i].type, barcodes[i].len, barcodes[i].data);
        free(barcodes[i].data);
      }
      free(barcodes);
    }else if(n_codes==0){
      printf("No barcodes\n");
    }else{
      printf("Read from Pixy error!\n");
    }
    while(1);*/
    wait_ms(50);
  }
}
