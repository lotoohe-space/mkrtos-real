################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/dietlibc-0.34/dyn_start.c \
../src/dietlibc-0.34/dyn_stop.c \
../src/dietlibc-0.34/stackgap-g.c 

OBJS += \
./src/dietlibc-0.34/dyn_start.o \
./src/dietlibc-0.34/dyn_stop.o \
./src/dietlibc-0.34/stackgap-g.o 

C_DEPS += \
./src/dietlibc-0.34/dyn_start.d \
./src/dietlibc-0.34/dyn_stop.d \
./src/dietlibc-0.34/stackgap-g.d 


# Each subdirectory must supply rules for building sources it contributes
src/dietlibc-0.34/%.o: ../src/dietlibc-0.34/%.c src/dietlibc-0.34/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -ffunction-sections -fdata-sections -fno-builtin -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative -Wall  -g -D__dietlibc__ -D__arm__ -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ls\src\dietlibc-0.34" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ls\src\dietlibc-0.34\include" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ls\src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

