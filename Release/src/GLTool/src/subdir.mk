################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/GLTool/src/GLBatch.cpp \
../src/GLTool/src/GLShaderManager.cpp \
../src/GLTool/src/GLTools.cpp \
../src/GLTool/src/GLTriangleBatch.cpp \
../src/GLTool/src/math3d.cpp 

OBJS += \
./src/GLTool/src/GLBatch.o \
./src/GLTool/src/GLShaderManager.o \
./src/GLTool/src/GLTools.o \
./src/GLTool/src/GLTriangleBatch.o \
./src/GLTool/src/math3d.o 

CPP_DEPS += \
./src/GLTool/src/GLBatch.d \
./src/GLTool/src/GLShaderManager.d \
./src/GLTool/src/GLTools.d \
./src/GLTool/src/GLTriangleBatch.d \
./src/GLTool/src/math3d.d 


# Each subdirectory must supply rules for building sources it contributes
src/GLTool/src/%.o: ../src/GLTool/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -DGAIN_MASK=1 -DVALIDATION_PERIOD_SECONDS=0 -DDISABLE_NEON_DEI=1 -DNO_ARM_NEON=1 -DENABLE_ENHANCE_FUNCTION=0 -DMVDECT=0 -DUSE_BMPCAP=0 -DWHOLE_PIC=1 -DUSE_GAIN=1 -DCAM_COUNT=8 -DUSE_CAP_SPI=0 -DTRACK_MODE=0 -DDOUBLE_SCREEN=0 -DTEST_GAIN=1 -DGSTREAM_CAP=0 -DUSE_UART=0 -DUSE_12=1 -DCOMM_UART=0 -I../src/OSA_CAP/inc -I../src/GLTool/include -I/usr/include/opencv -I/usr/include/gstreamer-1.0 -I/usr/include/GL -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I/usr/include -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -O3 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_50,code=sm_50 -m64 -odir "src/GLTool/src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -DGAIN_MASK=1 -DVALIDATION_PERIOD_SECONDS=0 -DDISABLE_NEON_DEI=1 -DNO_ARM_NEON=1 -DENABLE_ENHANCE_FUNCTION=0 -DMVDECT=0 -DUSE_BMPCAP=0 -DWHOLE_PIC=1 -DUSE_GAIN=1 -DCAM_COUNT=8 -DUSE_CAP_SPI=0 -DTRACK_MODE=0 -DDOUBLE_SCREEN=0 -DTEST_GAIN=1 -DGSTREAM_CAP=0 -DUSE_UART=0 -DUSE_12=1 -DCOMM_UART=0 -I../src/OSA_CAP/inc -I../src/GLTool/include -I/usr/include/opencv -I/usr/include/gstreamer-1.0 -I/usr/include/GL -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/glib-2.0/include -I/usr/lib/aarch64-linux-gnu/include -I/usr/include -I/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include -O3 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


