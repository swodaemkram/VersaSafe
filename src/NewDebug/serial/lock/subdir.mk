################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../serial/lock/apu_lock.c \
../serial/lock/secureip.c 

OBJS += \
./serial/lock/apu_lock.o \
./serial/lock/secureip.o 

C_DEPS += \
./serial/lock/apu_lock.d \
./serial/lock/secureip.d 


# Each subdirectory must supply rules for building sources it contributes
serial/lock/%.o: ../serial/lock/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


