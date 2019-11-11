#include <iostream>
#include "ReadBarcode.h"
#include "TextUtfEncoding.h"

using namespace ZXing;

extern "C" {

#include "pixy.h"

int pixy_decode_qr(
  uint16_t width, uint16_t height,
  uint8_t* rgb) {
  uint8_t *gray_scale = (uint8_t*)malloc(width*height);;
  uint8_t max_gray = 0;

  for(int i=0;i<height;i++){
    for(int j=0;j<width;j++){
      uint8_t r = rgb[(i*width+j)*3+0],
              g = rgb[(i*width+j)*3+1],
              b = rgb[(i*width+j)*3+2];
      uint8_t gray = (uint8_t)(r*2126/10000 + g*7152/10000 + b*722/10000);
      gray_scale[i*width+j] = gray;
      if(max_gray < gray)
	max_gray = gray;
    }
  }
  for(int i=0;i<height*width;i++){
    uint16_t corrected = gray_scale[i] * 255/max_gray;
    gray_scale[i] = (uint8_t) corrected;
  }
  
  auto result = ReadBarcode(width, height, gray_scale, width,
	{BarcodeFormatFromString("QR_CODE")}, true, true);
	
  free(gray_scale);
  
  if(result.isValid()){
	std::cout<<"Text: " << TextUtfEncoding::ToUtf8(result.text())<<"\n";
	return 1;
  } else {
	return 0;
  }
}

}
