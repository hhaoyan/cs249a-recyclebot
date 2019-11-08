#include <stdio.h>
#include <stdlib.h>

#include "pixy.h"

int main(void) {
  pixy_init();

  printf("Started\n");

  pixy_get_resolution();
  pixy_set_lamp(0, 0);

  while(1){
    pixy_block* blocks;
    int n_blocks = pixy_get_blocks(1, 0xff, &blocks);
    
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
           
    wait_ms(1000/30);
  }
}
