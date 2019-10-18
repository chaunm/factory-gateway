################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/data-handler/data-handler.c \
../src/data-handler/uart-data.c 

OBJS += \
./src/data-handler/data-handler.o \
./src/data-handler/uart-data.o 

C_DEPS += \
./src/data-handler/data-handler.d \
./src/data-handler/uart-data.d 


# Each subdirectory must supply rules for building sources it contributes
src/data-handler/%.o: ../src/data-handler/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	$(CC)gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


