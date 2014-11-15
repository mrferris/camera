#include "Camera.h"
#include <unistd.h>
#include <iostream>

int main(){

  Camera cam;
  printf("Created camera.\n");
  int i;

  IImageBitmap demosaic;
  IImageBitmap color;
  int file_number = 0;
  char file_number_string[3];
  std::string file_name;
  std::string demosaic_prefix = "demosaic/image";
  std::string color_prefix = "color/image";
  std::string file_suffix = ".bmp";

  for(file_number=0;file_number<50;file_number++){
    

    sprintf(file_number_string, "%d",file_number);
    file_name = demosaic_prefix + file_number_string + file_suffix;

    //    demosaic = cam.get_demosaic();
    //    cam.save_image(demosaic, file_name);
    color = cam.get_color_pipeline();

    file_name = color_prefix + file_number_string + file_suffix;
    cam.save_image(color,file_name);

    printf("Saved image number %d.\n",file_number);

  }



  return 0;

}
