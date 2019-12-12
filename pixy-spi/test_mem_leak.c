#include <stdio.h>
#include <stdlib.h>
#include <zbar.h>


int main(int argc, char** argv) {
    
  zbar_image_scanner_t * scanner = zbar_image_scanner_create();
  zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);

  uint8_t *gray_scale = malloc(316*208);
  FILE* file = fopen("/Users/huohaoyan/Downloads/zbar.bmp", "rb");
  fseek(file, 1078, 0);
  fread(gray_scale, 316*208, 1, file);
  fclose(file);
  uint8_t data[316];
  for(int i=0,j=316*207;i<j;){
    memcpy(data, gray_scale+i, 316);
    memcpy(gray_scale+i, gray_scale+j, 316);
    memcpy(gray_scale+j, data, 316);
    i+=316;
    j-=316;
  }

  for(int i=0;i<1000;i++){
    zbar_image_t *image = zbar_image_create();
    zbar_image_set_format(image, *(int*)"Y800");
    zbar_image_set_size(image, 316, 208);
    zbar_image_set_data(image, gray_scale, 316*208, NULL);
    
    // printf("%d\n", zbar_scan_image(scanner, image));
    zbar_scan_image(scanner, image);

    const zbar_symbol_t *symbol = zbar_image_first_symbol(image);
    for(; symbol; symbol = zbar_symbol_next(symbol)) {
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        const char *data = zbar_symbol_get_data(symbol);
        // printf("decoded %s symbol \"%s\"\n",
               // zbar_get_symbol_name(typ), data);
    }

    zbar_image_destroy(image);
  }
  free(gray_scale);

  zbar_image_scanner_destroy(scanner);

  debug_mem_finalize();

  return 0;
}
