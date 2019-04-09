################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../drivers/gsr40.cpp \
../drivers/jcm_driver.cpp \
../drivers/mei_driver.cpp \
../drivers/pelicano.cpp \
../drivers/ucd.cpp \
../drivers/ups.cpp \
../drivers/upsmon.cpp \
../drivers/vim.cpp 

OBJS += \
./drivers/gsr40.o \
./drivers/jcm_driver.o \
./drivers/mei_driver.o \
./drivers/pelicano.o \
./drivers/ucd.o \
./drivers/ups.o \
./drivers/upsmon.o \
./drivers/vim.o 

CPP_DEPS += \
./drivers/gsr40.d \
./drivers/jcm_driver.d \
./drivers/mei_driver.d \
./drivers/pelicano.d \
./drivers/ucd.d \
./drivers/ups.d \
./drivers/upsmon.d \
./drivers/vim.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


