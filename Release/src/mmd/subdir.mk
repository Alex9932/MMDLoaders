################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/mmd/pmx.cpp \
../src/mmd/rg_mmd_loader.cpp \
../src/mmd/rg_mmd_reader.cpp \
../src/mmd/rg_pmd.cpp \
../src/mmd/rg_vmd.cpp \
../src/mmd/utils.cpp \
../src/mmd/vpd.cpp 

CPP_DEPS += \
./src/mmd/pmx.d \
./src/mmd/rg_mmd_loader.d \
./src/mmd/rg_mmd_reader.d \
./src/mmd/rg_pmd.d \
./src/mmd/rg_vmd.d \
./src/mmd/utils.d \
./src/mmd/vpd.d 

OBJS += \
./src/mmd/pmx.o \
./src/mmd/rg_mmd_loader.o \
./src/mmd/rg_mmd_reader.o \
./src/mmd/rg_pmd.o \
./src/mmd/rg_vmd.o \
./src/mmd/utils.o \
./src/mmd/vpd.o 


# Each subdirectory must supply rules for building sources it contributes
src/mmd/%.o: ../src/mmd/%.cpp src/mmd/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/bullet -I/usr/include/opencv4 -I"/mnt/hentai/engine_workspace/engine/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-mmd

clean-src-2f-mmd:
	-$(RM) ./src/mmd/pmx.d ./src/mmd/pmx.o ./src/mmd/rg_mmd_loader.d ./src/mmd/rg_mmd_loader.o ./src/mmd/rg_mmd_reader.d ./src/mmd/rg_mmd_reader.o ./src/mmd/rg_pmd.d ./src/mmd/rg_pmd.o ./src/mmd/rg_vmd.d ./src/mmd/rg_vmd.o ./src/mmd/utils.d ./src/mmd/utils.o ./src/mmd/vpd.d ./src/mmd/vpd.o

.PHONY: clean-src-2f-mmd

