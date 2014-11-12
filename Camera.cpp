#include "Camera.h"
#include <iostream>

Camera::Camera(){

  gige::InitGigEVisionAPI();
  gige::InitImageProcAPI();
  gige_api = gige::GetGigEVisionAPI();
  image_proc = gige::GetImageProcAPI();
  
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

  if(device != NULL && device->Connect()){
    
    // enable trigger mode
    bool status = device->SetStringNodeValue("TriggerMode", "On");
    // set software trigger mode
    status = device->SetStringNodeValue("TriggerSource", "Software");
    status = device->SetStringNodeValue("AcquisitionMode","Continuous");
    // start acquisition
    status = device->SetIntegerNodeValue("TLParamsLocked", 1);
    status = device->CommandNodeExecute("AcquisitionStart");

  }

  saveImageAlg = image_proc -> GetAlgorithmByName("FileSave");
  demosaicAlg = image_proc -> GetAlgorithmByName("DemosaicBilinear");
  
  saveImageAlg->CreateParams(&saveImageParams);
  image_proc -> CreateBitmap(&demosaicBitmap);

  saveImageParams->SetStringNodeValue("FilePath","EdisonPicture");


}

Camera::~Camera(){

  device->CommandNodeExecute("AcquisitionStop");
  device->Disconnect();

  gige::ExitGigEVisionAPI();
  gige::ExitImageProcAPI();



}


void Camera::get_image(){
  device->CommandNodeExecute("TriggerSoftware");
  printf("In get_image.\n");

  if(!device->IsBufferEmpty()){

    printf("Getting Image.");
    gige::IImageInfo image_info;
    device->GetImageInfo(&image_info);
    printf("Got image info.\n");

    if(image_info != NULL){

      UINT32 sizeX, sizeY;
      image_info->GetSize(sizeX,sizeY);
      std::cout << "X:" << sizeX << " Y: "<< sizeY << std::endl;

      if(!image_proc->ExecuteAlgorithm(demosaicAlg,image_info,demosaicBitmap)){
	printf("Failed to demosaic.\n");
      }
      else{
	printf("Demosaic.\n");
      }

      if(!image_proc->ExecuteAlgorithm(saveImageAlg,demosaicBitmap,gige::IImageBitmap(),saveImageParams)){
	printf("Failed to save.\n");
      }
      else{
	printf("saved.\n");
      }
      printf("\n");


      device->PopImage(image_info);
    }

  }
  else{
    printf("Device buffer empty.\n");
  }


}
