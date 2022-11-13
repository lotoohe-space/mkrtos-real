################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/dietlibc-0.34/libshell/basename.c \
../src/dietlibc-0.34/libshell/dirname.c \
../src/dietlibc-0.34/libshell/fnmatch.c \
../src/dietlibc-0.34/libshell/glob.c \
../src/dietlibc-0.34/libshell/realpath.c 

OBJS += \
./src/dietlibc-0.34/libshell/basename.o \
./src/dietlibc-0.34/libshell/dirname.o \
./src/dietlibc-0.34/libshell/fnmatch.o \
./src/dietlibc-0.34/libshell/glob.o \
./src/dietlibc-0.34/libshell/realpath.o 

C_DEPS += \
./src/dietlibc-0.34/libshell/basename.d \
./src/dietlibc-0.34/libshell/dirname.d \
./src/dietlibc-0.34/libshell/fnmatch.d \
./src/dietlibc-0.34/libshell/glob.d \
./src/dietlibc-0.34/libshell/realpath.d 


# Each subdirectory must supply rules for building sources it contributes
src/dietlibc-0.34/libshell/%.o: ../src/dietlibc-0.34/libshell/%.c src/dietlibc-0.34/libshell/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mthumb-interwork -O0 -ffunction-sections -fdata-sections -fno-builtin -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative -Wall  -g -D__dietlibc__ -D__arm__ -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ym\src\dietlibc-0.34" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ym\src\dietlibc-0.34\include" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ym\src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


