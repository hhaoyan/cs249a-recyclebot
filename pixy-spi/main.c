#include <stdio.h>
#include <stdlib.h>

#include "pixy.h"

int main(void) {
  pixy_init();

  printf("Started\n");

  uint16_t width, height;
  pixy_get_resolution(&width, &height);
  pixy_set_lamp(0, 0);

  uint8_t* image = malloc(width * height * 3);
  FILE* file;
  while(1){
    /*
    pixy_block* blocks;
    int n_blocks = pixy_get_blocks(1, 0xff, &blocks);
    wait_ms(500);
    n_blocks = pixy_get_blocks(1, 0xff, &blocks);
    printf("Found %d blocks\n", n_blocks);
    wait_ms(500);
    n_blocks = pixy_get_blocks(1, 0xff, &blocks); 
    printf("Found %d blocks\n", n_blocks);
    
    wait_ms(500);
    n_blocks = pixy_get_blocks(1, 0xff, &blocks); 
    printf("Found %d blocks\n", n_blocks);
    
    if(n_blocks >= 0){
      printf("Found %d blocks\n", n_blocks);
      for(int i=0;i<n_blocks;i++){
        pixy_block* block = blocks+i;
        printf("Block sig: %d, center: %d,%d; size: %dx%d, color angle: %d,"
               "traking index: %d, age: %d\n",
               block->sig, block->x, block->y, block->width, block->height, 
               block->color_angle, block->track_index, block->age);
      }
      free(blocks);
    }
    */
    printf("Start reading... \n");
    printf("%d pixels\n", 
      pixy_get_image(width, height, image));
    printf("Found %d QR codes\n",
      pixy_decode_qr(width, height, image));
    pixy_write_image("image.bmp", width, height, image);

    wait_ms(1000);
  }
}
