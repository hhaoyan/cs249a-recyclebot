#ifndef _PIXY_H
#define _PIXY_H

#include <stdint.h>

// Functions for hareware implementation
extern void wait_us(int us);
extern void wait_ms(int ms);
extern void spi_init();
extern void spi_trasfer_data(
  uint8_t* buf_write, uint8_t write_size, 
  uint8_t* buf_read, uint8_t read_size);

// Pixy related functions
extern void pixy_init();
extern void pixy_packet_send(
  uint8_t* data, uint8_t len, uint8_t packet_type, int do_checksum);
extern int pixy_packet_recv(
  uint8_t** data, uint8_t* len, uint8_t* packet_type);

typedef struct pixy_block {
  uint16_t sig;
  uint16_t x, y;
  uint16_t width, height;
  int16_t color_angle;
  uint8_t track_index;
  uint8_t age;
} pixy_block;

typedef enum {
  LINE_FEA_VECTOR = 0x1,
  LINE_FEA_INTERSECTION = 0x2,
  LINE_FEA_BARCODE = 0x4,
} line_feature_type;

typedef enum {
  LINE_FEA_REQ_MAIN = 0x0,
  LINE_FEA_REQ_ALL = 0x1,
} line_feature_req;

typedef struct line_feature {
  line_feature_type type;
  uint8_t len;
  uint8_t *data;
} line_feature;

typedef enum {
  BARCODE_EAN8 = 8,
  BARCODE_I25 = 25,
  BARCODE_CODE39 = 39,
  BARCODE_QRCODE = 64,
  BAERCODE_CODE128 = 128,
} barcode_type;

typedef struct barcode {
  barcode_type type;
  uint8_t len;
  uint8_t *data;
} barcode;

// Pixy helper functions
// Connect with Pixy and print version information.
extern void pixy_get_version();
// Get Pixy cam image resolution
extern void pixy_get_resolution(uint16_t* width, uint16_t* height);
// Set lamp brightness, 1 for on and 0 for off.
extern void pixy_set_lamp(int upper, int lower);
// Get blocks recognized by Pixy cam, return number of blocks or -1 on error.
extern int pixy_get_blocks(
  uint8_t sigmap, uint8_t max_blocks, pixy_block** blocks);
// Get line features, return number of features or -1 on error.
extern int pixy_get_line_features(
  line_feature_req req_group, line_feature_req req_type, line_feature** features);
// Get bar codes, return number of barcodes or -1 on error.
extern int pixy_get_barcodes(barcode** codes);
// Get the raw image by RGB values, return number of pixels read.
extern int pixy_get_image(
  uint16_t width, uint16_t height, 
  uint8_t* rgb);
extern int pixy_write_image(
  const char* filename,
  uint16_t width, uint16_t height, 
  uint8_t* rgb);
extern int pixy_decode_qr(
  uint16_t width, uint16_t height,
  uint8_t* rgb);

#endif
