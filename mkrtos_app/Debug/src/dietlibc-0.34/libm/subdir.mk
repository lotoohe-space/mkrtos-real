################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/dietlibc-0.34/libm/acosh.c \
../src/dietlibc-0.34/libm/asinh.c \
../src/dietlibc-0.34/libm/atanh.c \
../src/dietlibc-0.34/libm/bessel.c \
../src/dietlibc-0.34/libm/cosh.c \
../src/dietlibc-0.34/libm/erf.c \
../src/dietlibc-0.34/libm/gamma.c \
../src/dietlibc-0.34/libm/ipow.c \
../src/dietlibc-0.34/libm/modf.c \
../src/dietlibc-0.34/libm/poly.c \
../src/dietlibc-0.34/libm/pow.c \
../src/dietlibc-0.34/libm/rint.c \
../src/dietlibc-0.34/libm/sinh.c \
../src/dietlibc-0.34/libm/tanh.c 

OBJS += \
./src/dietlibc-0.34/libm/acosh.o \
./src/dietlibc-0.34/libm/asinh.o \
./src/dietlibc-0.34/libm/atanh.o \
./src/dietlibc-0.34/libm/bessel.o \
./src/dietlibc-0.34/libm/cosh.o \
./src/dietlibc-0.34/libm/erf.o \
./src/dietlibc-0.34/libm/gamma.o \
./src/dietlibc-0.34/libm/ipow.o \
./src/dietlibc-0.34/libm/modf.o \
./src/dietlibc-0.34/libm/poly.o \
./src/dietlibc-0.34/libm/pow.o \
./src/dietlibc-0.34/libm/rint.o \
./src/dietlibc-0.34/libm/sinh.o \
./src/dietlibc-0.34/libm/tanh.o 

C_DEPS += \
./src/dietlibc-0.34/libm/acosh.d \
./src/dietlibc-0.34/libm/asinh.d \
./src/dietlibc-0.34/libm/atanh.d \
./src/dietlibc-0.34/libm/bessel.d \
./src/dietlibc-0.34/libm/cosh.d \
./src/dietlibc-0.34/libm/erf.d \
./src/dietlibc-0.34/libm/gamma.d \
./src/dietlibc-0.34/libm/ipow.d \
./src/dietlibc-0.34/libm/modf.d \
./src/dietlibc-0.34/libm/poly.d \
./src/dietlibc-0.34/libm/pow.d \
./src/dietlibc-0.34/libm/rint.d \
./src/dietlibc-0.34/libm/sinh.d \
./src/dietlibc-0.34/libm/tanh.d 


# Each subdirectory must supply rules for building sources it contributes
src/dietlibc-0.34/libm/%.o: ../src/dietlibc-0.34/libm/%.c src/dietlibc-0.34/libm/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -ffunction-sections -fdata-sections -fno-builtin -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative -Wall  -g -D__dietlibc__ -D__arm__ -I"D:\Project\MyProject1\mkrtos_real\mkrtos_app\src\dietlibc-0.34" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_app\src\dietlibc-0.34\include" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_app\src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


