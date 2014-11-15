OBJS = Camera.o cameratest.cpp GigEVisionSDK.cpp.o
CC = g++
GIGE_SDK = /usr/local/src/GigEVisionSDK/include


cameratest: $(OBJS)
	$(CC) -L/usr/local/lib/ -lGigEVisionSDK $(OBJS) -o cameratest

Camera.o: Camera.h Camera.cpp 
	$(CC) -c Camera.cpp Camera.h 
#Common.cpp.o


#GigE.o: $(GIGE_SDK)/gige_cpp/GigEVisionSDK.cpp $(GIGE_SDK)/gige_cpp/GigEVisionSDK.h $(GIGE_SDK)/gige_c/IGigEVisionAPI.h
#	$(CC) -c $(GIGE_SDK)/gige_cpp/GigEVisionSDK.cpp