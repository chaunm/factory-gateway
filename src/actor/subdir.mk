################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/actor/actor.c \
../src/actor/factory-actor.c 

OBJS += \
./src/actor/actor.o \
./src/actor/factory-actor.o 

C_DEPS += \
./src/actor/actor.d \
./src/actor/factory-actor.d 


# Each subdirectory must supply rules for building sources it contributes
src/actor/%.o: ../src/actor/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	$(CC)gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


