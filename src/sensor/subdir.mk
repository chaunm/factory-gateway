################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/sensor/sensor.c 

OBJS += \
./src/sensor/sensor.o 

C_DEPS += \
./src/sensor/sensor.d 


# Each subdirectory must supply rules for building sources it contributes
src/sensor/%.o: ../src/sensor/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	$(CC)gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


