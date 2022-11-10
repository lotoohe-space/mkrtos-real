################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/dietlibc-0.34/profiling/__mcount.c \
../src/dietlibc-0.34/profiling/monitor.c \
../src/dietlibc-0.34/profiling/profil.c 

OBJS += \
./src/dietlibc-0.34/profiling/__mcount.o \
./src/dietlibc-0.34/profiling/monitor.o \
./src/dietlibc-0.34/profiling/profil.o 

C_DEPS += \
./src/dietlibc-0.34/profiling/__mcount.d \
./src/dietlibc-0.34/profiling/monitor.d \
./src/dietlibc-0.34/profiling/profil.d 


# Each subdirectory must supply rules for building sources it contributes
src/dietlibc-0.34/profiling/%.o: ../src/dietlibc-0.34/profiling/%.c src/dietlibc-0.34/profiling/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -ffunction-sections -fdata-sections -fno-builtin -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative -Wall  -g -D__dietlibc__ -D__arm__ -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ls\src\dietlibc-0.34" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ls\src\dietlibc-0.34\include" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ls\src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


