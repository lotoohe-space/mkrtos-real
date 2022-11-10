################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ymodem/receive.c \
../src/ymodem/send.c \
../src/ymodem/ymodem.c 

OBJS += \
./src/ymodem/receive.o \
./src/ymodem/send.o \
./src/ymodem/ymodem.o 

C_DEPS += \
./src/ymodem/receive.d \
./src/ymodem/send.d \
./src/ymodem/ymodem.d 


# Each subdirectory must supply rules for building sources it contributes
src/ymodem/%.o: ../src/ymodem/%.c src/ymodem/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -ffunction-sections -fdata-sections -fno-builtin -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative -Wall  -g -D__dietlibc__ -D__arm__ -I"C:\Users\Administrator\eclipse-workspace\.metadata\mkrtos_app\src\dietlibc-0.34" -I"C:\Users\Administrator\eclipse-workspace\.metadata\mkrtos_app\src\dietlibc-0.34\include" -I"C:\Users\Administrator\eclipse-workspace\.metadata\mkrtos_app\src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


