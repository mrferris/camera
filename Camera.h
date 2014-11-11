#include <GigEVisionSDK.h>
#include <iostream>
#include <stdio.h>


class Camera {

 public:
  Camera();
  ~Camera();
  void get_image();
  
 private:
  gige::IGigEVisionAPI gige_api;
  gige::IImageProcAPI image_proc;
  gige::IDevice device;
  gige::IImageBitmap demosaicBitmap;
  gige::IAlgorithm demosaicAlg;
  gige::IParams saveImageParams;
  gige::IAlgorithm saveImageAlg;

};
