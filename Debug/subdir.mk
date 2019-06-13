################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../EnhStateGLRenderBridge.cpp\
../EnhStateMachine.cpp\
../AlarmTarget.cpp \
../BMPCaptureGroup.cpp \
../Camera.cpp \
../Cap_Spi_Message.cpp \
../CaptureGroup.cpp \
../CenterPoint_caculate.cpp \
../CheckMyself.cpp \
../ChosenCaptureGroup.cpp \
../ClicktoMoveForesight.cpp \
../CornerMarker.cpp \
../DataofAlarmarea.cpp \
../DynamicTrack.cpp \
../ExposureCompensationThread.cpp \
../FBOManager.cpp \
../FishCalib.cpp \
../ForeSight.cpp \
../GLEnv.cpp \
../GLRender.cpp \
../GetScreenBUffer.cpp \
../HDV4lcap.cpp \
../LineofRuler.cpp \
../Loader.cpp \
../MiscCaptureGroup.cpp \
../MvDetect.cpp \
../OitVehicle.cpp \
../PBOManager.cpp \
../PBO_FBO_Facade.cpp \
../PanoCaptureGroup.cpp \
../Parayml.cpp \
../PresetCameraGroup.cpp \
../ProcessIPCMsg.cpp \
../RenderDrawBehvrImpl.cpp \
../RenderMain.cpp \
../Render_Agent.cpp \
../STLASCIILoader.cpp \
../SelfCheckThread.cpp \
../Serial_port.cpp \
../ShaderParamArrays.cpp \
../Thread_Priority.cpp \
../Xin_IPC_Yuan_Recv_Message.cpp \
../Zodiac_GPIO_Message.cpp \
../Zodiac_Message.cpp \
../buffer.cpp \
../common.cpp \
../deinterlacing.cpp \
../doubleScreen.cpp \
../glInfo.cpp \
../glm.cpp \
../gst_capture.cpp \
../main.cpp \
../overLap.cpp \
../overLapBuffer.cpp \
../overLapRegion.cpp \
../pboProcessSrcThread.cpp \
../recvUARTdata.cpp \
../recvwheeldata.cpp \
../scanner.cpp \
../set_button.cpp \
../spiH.cpp \
../thread.cpp \
../thread_idle.cpp \
../updatebuffer.cpp \
../v4l2camera.cpp \
../yuv2rgb_neon.cpp 

OBJ_SRCS += \
../tank1215_b_m1.obj 

OBJS += \
./EnhStateGLRenderBridge.o\
./EnhStateMachine.o\
./AlarmTarget.o \
./BMPCaptureGroup.o \
./Camera.o \
./Cap_Spi_Message.o \
./CaptureGroup.o \
./CenterPoint_caculate.o \
./CheckMyself.o \
./ChosenCaptureGroup.o \
./ClicktoMoveForesight.o \
./CornerMarker.o \
./DataofAlarmarea.o \
./DynamicTrack.o \
./ExposureCompensationThread.o \
./FBOManager.o \
./FishCalib.o \
./ForeSight.o \
./GLEnv.o \
./GLRender.o \
./GetScreenBUffer.o \
./HDV4lcap.o \
./LineofRuler.o \
./Loader.o \
./MiscCaptureGroup.o \
./MvDetect.o \
./OitVehicle.o \
./PBOManager.o \
./PBO_FBO_Facade.o \
./PanoCaptureGroup.o \
./Parayml.o \
./PresetCameraGroup.o \
./ProcessIPCMsg.o \
./RenderDrawBehvrImpl.o \
./RenderMain.o \
./Render_Agent.o \
./STLASCIILoader.o \
./SelfCheckThread.o \
./Serial_port.o \
./ShaderParamArrays.o \
./Thread_Priority.o \
./Xin_IPC_Yuan_Recv_Message.o \
./Zodiac_GPIO_Message.o \
./Zodiac_Message.o \
./buffer.o \
./common.o \
./deinterlacing.o \
./doubleScreen.o \
./glInfo.o \
./glm.o \
./gst_capture.o \
./main.o \
./overLap.o \
./overLapBuffer.o \
./overLapRegion.o \
./pboProcessSrcThread.o \
./recvUARTdata.o \
./recvwheeldata.o \
./scanner.o \
./set_button.o \
./spiH.o \
./thread.o \
./thread_idle.o \
./updatebuffer.o \
./v4l2camera.o \
./yuv2rgb_neon.o 

CPP_DEPS += \
./EnhStateGLRenderBridge.d\
./EnhStateMachine.d\
./AlarmTarget.d \
./BMPCaptureGroup.d \
./Camera.d \
./Cap_Spi_Message.d \
./CaptureGroup.d \
./CenterPoint_caculate.d \
./CheckMyself.d \
./ChosenCaptureGroup.d \
./ClicktoMoveForesight.d \
./CornerMarker.d \
./DataofAlarmarea.d \
./DynamicTrack.d \
./ExposureCompensationThread.d \
./FBOManager.d \
./FishCalib.d \
./ForeSight.d \
./GLEnv.d \
./GLRender.d \
./GetScreenBUffer.d \
./HDV4lcap.d \
./LineofRuler.d \
./Loader.d \
./MiscCaptureGroup.d \
./MvDetect.d \
./OitVehicle.d \
./PBOManager.d \
./PBO_FBO_Facade.d \
./PanoCaptureGroup.d \
./Parayml.d \
./PresetCameraGroup.d \
./ProcessIPCMsg.d \
./RenderDrawBehvrImpl.d \
./RenderMain.d \
./Render_Agent.d \
./STLASCIILoader.d \
./SelfCheckThread.d \
./Serial_port.d \
./ShaderParamArrays.d \
./Thread_Priority.d \
./Xin_IPC_Yuan_Recv_Message.d \
./Zodiac_GPIO_Message.d \
./Zodiac_Message.d \
./buffer.d \
./common.d \
./deinterlacing.d \
./doubleScreen.d \
./glInfo.d \
./glm.d \
./gst_capture.d \
./main.d \
./overLap.d \
./overLapBuffer.d \
./overLapRegion.d \
./pboProcessSrcThread.d \
./recvUARTdata.d \
./recvwheeldata.d \
./scanner.d \
./set_button.d \
./spiH.d \
./thread.d \
./thread_idle.d \
./updatebuffer.d \
./v4l2camera.d \
./yuv2rgb_neon.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -DDISABLE_NEON_DEI=1 -DNO_ARM_NEON=1 -DMVDECT=0 -DWHOLE_PIC=1 -DUSE_BMPCAP=0 -DCAM_COUNT=8 -DTRACK_MODE=0 -DUSE_CAP_SPI=0 -DUSE_UART=0 -DDOUBLE_SCREEN=0 -DENABLE_ENHANCE_FUNCTION=0 -DTEST_GAIN=1 -DUSE_12=1 -DUSE_GAIN=1 -DGSTREAM_CAP=1 -I../OSA_CAP/inc -I/usr/include/GL -I../GLTool/include -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I/usr/include -I/usr/include/opencv -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -G -g -O0 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_50,code=sm_50 -m64 -odir "." -M -o "$(@:%.o=%.d)" "$<"

	/usr/local/cuda-8.0/bin/nvcc -DDISABLE_NEON_DEI=1 -DNO_ARM_NEON=1 -DMVDECT=0 -DWHOLE_PIC=1 -DDOUBLE_SCREEN=0 -DUSE_BMPCAP=0 -DCAM_COUNT=8 -DUSE_CAP_SPI=0 -DTRACK_MODE=0 -DUSE_UART=0 -DENABLE_ENHANCE_FUNCTION=0 -DTEST_GAIN=1 -DUSE_12=1 -DUSE_GAIN=1 -DGSTREAM_CAP=1 -I../OSA_CAP/inc -I/usr/include/GL -I../GLTool/include -I/usr/include/gstreamer-1.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I/usr/include -I/usr/include/opencv -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -G -g -O0 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


