################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../serial/rs485.cpp \
../serial/serial.cpp \
../serial/trim.cpp 

OBJS += \
./serial/rs485.o \
./serial/serial.o \
./serial/trim.o 

CPP_DEPS += \
./serial/rs485.d \
./serial/serial.d \
./serial/trim.d 


# Each subdirectory must supply rules for building sources it contributes
serial/%.o: ../serial/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


