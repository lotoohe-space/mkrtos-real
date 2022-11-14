################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/heap_stack.c \
../src/receive.c \
../src/serial.c 

OBJS += \
./src/heap_stack.o \
./src/receive.o \
./src/serial.o 

C_DEPS += \
./src/heap_stack.d \
./src/receive.d \
./src/serial.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mthumb-interwork -Os -ffunction-sections -fdata-sections -fno-builtin -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative -Wall  -g -D__dietlibc__ -D__arm__ -I"D:\Project\MyProject1\mkrtos_real\mkrtos_sdk\src\dietlibc-0.34" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_sdk\src\dietlibc-0.34\include" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ym\src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


