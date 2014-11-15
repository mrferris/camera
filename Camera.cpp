#include "Camera.h"
#include <unistd.h>
#include <iostream>
bool done;
Camera::Camera(){

  gige::InitGigEVisionAPI();
  gige::InitImageProcAPI();
  gige_api = gige::GetGigEVisionAPI();
  image_proc = gige::GetImageProcAPI();
  done = false;
  if(!gige_api->IsUsingKernelDriver()){
    std::cerr << "Not using filter driver, we should install that at some point." << std::endl;
  }

  gige_api->SetHeartbeatTime(3);
  gige_api->FindAllDevices(1.0);
  const gige::DevicesList devices = gige_api->GetAllDevices();

  if(devices.size() > 0){
    device = devices[0];
  }
  else{
    printf("Found %d devices.\n",devices.size());
  }
  device -> ClearImageBuffer();
  if(device != NULL && device->Connect()){
    
    printf("Starting acqusition.\n");
    device -> ClearImageBuffer();
    device -> CommandNodeExecute("AcquisitionStop");
    // enable trigger mode
    bool status = device->SetStringNodeValue("TriggerMode", "On");
    // set software trigger mode
    status = device->SetStringNodeValue("TriggerSource", "Software");
    status = device->SetStringNodeValue("AcquisitionMode","Continuous");
    // start acquisition
    status = device->SetIntegerNodeValue("TLParamsLocked", 1);
    status = device->CommandNodeExecute("AcquisitionStart");

  }

  INode node = device->GetNode("ExposureTime");
  node -> SetFloatNodeValue(5000);
  double value;
  node->GetFloatNodeValue(value);
  printf("exposure time: %f\n",value);
  

}


Camera::~Camera(){
  
  device->CommandNodeExecute("AcquisitionStop");
  device->Disconnect();

  gige::ExitGigEVisionAPI();
  gige::ExitImageProcAPI();
  device -> ClearImageBuffer();

}



/*
 *Returns IImageBitmap containing bitmap of demosaic image. 
 *Returns NULL if no image available.
 */
IImageBitmap Camera::get_demosaic(){


  device -> CommandNodeExecute("TriggerSoftware");
  
  //Get demosaic algorithm from image processing library.
  IAlgorithm demosaic_alg = image_proc->GetAlgorithmByName("DemosaicBilinear");
  
  IImageBitmap demosaic_bitmap;
  image_proc -> CreateBitmap(&demosaic_bitmap);

  IImageInfo image_info;  
  while(device->IsBufferEmpty()){
    usleep(1000);
  }
  if(!device->IsBufferEmpty()){
    
    device->GetImageInfo(&image_info);
    if(image_info != NULL){

      //Get demosaic image, put into demosaic_bitmap.
      image_proc -> ExecuteAlgorithm(demosaic_alg, image_info, demosaic_bitmap);

    }
    else{
      printf("Image info is NULL.\n");
      return NULL;
    }

  }
  else{
    printf("Device buffer is empty.\n");
    return NULL;
  }

  device -> PopImage(image_info);

  return demosaic_bitmap;

}




IImageBitmap Camera::get_color_pipeline(){

  device -> CommandNodeExecute("TriggerSoftware");
  printf("Executed trigger.\n");
  //get algorithm
  IAlgorithm color_pipeline_alg = image_proc->GetAlgorithmByName("ColorPipeline");

  //Sharpen algo & params
  IAlgorithm sharpen_alg = image_proc -> GetAlgorithmByName("Sharpen");
  IParams sharpen_params;
  IImageBitmap sharpen_bitmap;
  image_proc->CreateBitmap(&sharpen_bitmap);
  sharpen_alg -> CreateParams(&sharpen_params);

  //create algorithm parameters
  IParams color_pipeline_params;
  color_pipeline_alg -> CreateParams(&color_pipeline_params);

  //create algorithm results (what is this for?)
  IResults color_pipeline_results;
  color_pipeline_alg -> CreateResults(&color_pipeline_results);

  //create resulting bitmap
  IImageBitmap color_pipeline_bitmap;
  image_proc -> CreateBitmap(&color_pipeline_bitmap);
  
  IImageInfo image_info;
  device->GetImageInfo(&image_info);
  printf("Got image info.\n");

  /*  while(device->IsBufferEmpty()){
    usleep(1000);
    printf("No image yet.\n");
    
    }*/
  if(!device->IsBufferEmpty()){
    if(image_info != NULL){

      image_proc -> ExecuteAlgorithm(color_pipeline_alg, image_info, color_pipeline_bitmap, color_pipeline_params,
				     color_pipeline_results);
      image_proc -> ExecuteAlgorithm(sharpen_alg,color_pipeline_bitmap, sharpen_bitmap, sharpen_params, IResults());
      

    }
    else{
      printf("Image info is NULL.\n");
      return NULL;
    }

  }
  else{
    printf("Device buffer empty.\n");
    return NULL;
  }

  device -> PopImage(image_info);
  return sharpen_bitmap;

}


void Camera::save_image(IImageBitmap image, std::string file_name){
  
  IAlgorithm save_image_alg = image_proc->GetAlgorithmByName("FileSave");
  IParams save_image_params;
  save_image_alg->CreateParams(&save_image_params);

  save_image_params->SetStringNodeValue("FilePath",file_name);
  if(!image_proc->ExecuteAlgorithm(save_image_alg,image,IImageBitmap(),save_image_params)){

    printf("Failed to save to file: ");
    printf(file_name.c_str());
    printf(".\n");

  }

}


