################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../buslogic.cpp \
../common.cpp \
../dbf.cpp \
../help.cpp \
../ip_module.cpp \
../logs.cpp \
../net_gateway.cpp \
../pid.cpp \
../queue.cpp \
../segfault.cpp \
../socket_class.cpp \
../trim.cpp \
../usb.cpp \
../usb_gateway.cpp \
../versasafe.cpp 

OBJS += \
./buslogic.o \
./common.o \
./dbf.o \
./help.o \
./ip_module.o \
./logs.o \
./net_gateway.o \
./pid.o \
./queue.o \
./segfault.o \
./socket_class.o \
./trim.o \
./usb.o \
./usb_gateway.o \
./versasafe.o 

CPP_DEPS += \
./buslogic.d \
./common.d \
./dbf.d \
./help.d \
./ip_module.d \
./logs.d \
./net_gateway.d \
./pid.d \
./queue.d \
./segfault.d \
./socket_class.d \
./trim.d \
./usb.d \
./usb_gateway.d \
./versasafe.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


