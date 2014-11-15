#include <GigEVisionSDK.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

using namespace gige;

class Camera {

 public:
  Camera();
  ~Camera();
  
  IImageBitmap get_demosaic();
  IImageBitmap get_color_pipeline();
  void save_image(IImageBitmap image, std::string file_name);
  float get_exposure_time();
  
 private:
  IGigEVisionAPI gige_api;
  IImageProcAPI image_proc;
  IDevice device;
  /*  IImageBitmap demosaicBitmap;
  IAlgorithm demosaicAlg;
  IParams saveImageParams;
  IAlgorithm saveImageAlg;
  IResults pipelineresults;*/
  

};
