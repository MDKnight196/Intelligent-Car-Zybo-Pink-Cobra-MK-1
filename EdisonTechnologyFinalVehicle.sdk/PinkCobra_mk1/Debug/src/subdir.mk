################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/PIDcontroller.c \
../src/PinkCorbra_mk1.c \
../src/bluetooth.c \
../src/hullEffect.c \
../src/interupts.c \
../src/oLED.c \
../src/operationalMode.c \
../src/pixy.c \
../src/platform.c \
../src/utrasonic.c 

OBJS += \
./src/PIDcontroller.o \
./src/PinkCorbra_mk1.o \
./src/bluetooth.o \
./src/hullEffect.o \
./src/interupts.o \
./src/oLED.o \
./src/operationalMode.o \
./src/pixy.o \
./src/platform.o \
./src/utrasonic.o 

C_DEPS += \
./src/PIDcontroller.d \
./src/PinkCorbra_mk1.d \
./src/bluetooth.d \
./src/hullEffect.d \
./src/interupts.d \
./src/oLED.d \
./src/operationalMode.d \
./src/pixy.d \
./src/platform.d \
./src/utrasonic.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM v7 gcc compiler'
	arm-none-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -I../../PinkCobra_mk1_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


