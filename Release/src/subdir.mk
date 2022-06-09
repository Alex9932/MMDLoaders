################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/animation.cpp \
../src/animator.cpp \
../src/camera.cpp \
../src/gbuffer.cpp \
../src/main.cpp \
../src/pm2_loader.cpp \
../src/postprocess.cpp \
../src/renderer.cpp \
../src/rl3d.cpp 

CPP_DEPS += \
./src/animation.d \
./src/animator.d \
./src/camera.d \
./src/gbuffer.d \
./src/main.d \
./src/pm2_loader.d \
./src/postprocess.d \
./src/renderer.d \
./src/rl3d.d 

OBJS += \
./src/animation.o \
./src/animator.o \
./src/camera.o \
./src/gbuffer.o \
./src/main.o \
./src/pm2_loader.o \
./src/postprocess.o \
./src/renderer.o \
./src/rl3d.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/bullet -I/usr/include/opencv4 -I"/mnt/hentai/engine_workspace/engine/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/animation.d ./src/animation.o ./src/animator.d ./src/animator.o ./src/camera.d ./src/camera.o ./src/gbuffer.d ./src/gbuffer.o ./src/main.d ./src/main.o ./src/pm2_loader.d ./src/pm2_loader.o ./src/postprocess.d ./src/postprocess.o ./src/renderer.d ./src/renderer.o ./src/rl3d.d ./src/rl3d.o

.PHONY: clean-src

