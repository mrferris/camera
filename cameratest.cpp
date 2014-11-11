#include "Camera.h"
#include <unistd.h>

int main(){

  Camera cam;
  while(true){
    cam.get_image();
    sleep(1);
  }

  return 0;

}
