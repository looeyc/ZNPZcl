################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/profiles.c \
../src/zcldatatype.c \
../src/zclframe.c \
../src/zclmsg.c \
../src/zclmsgfifo.c \
../src/znpzcl.c 

OBJS += \
./src/profiles.o \
./src/zcldatatype.o \
./src/zclframe.o \
./src/zclmsg.o \
./src/zclmsgfifo.o \
./src/znpzcl.o 

C_DEPS += \
./src/profiles.d \
./src/zcldatatype.d \
./src/zclframe.d \
./src/zclmsg.d \
./src/zclmsgfifo.d \
./src/znpzcl.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-arago-linux-gnueabi-gcc -I"/home/chengm/myLinux/eclipse_prjs/ZNPZcl/inc" -I"/home/chengm/myLinux/eclipse_prjs/ZNPLib/inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


