################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/__main.c \
../src/fmt_8long.c \
../src/fmt_str.c \
../src/fmt_ulongpadright.c \
../src/heap_stack.c \
../src/ls.c \
../src/reloc.c 

S_UPPER_SRCS += \
../src/start.S 

OBJS += \
./src/__main.o \
./src/fmt_8long.o \
./src/fmt_str.o \
./src/fmt_ulongpadright.o \
./src/heap_stack.o \
./src/ls.o \
./src/reloc.o \
./src/start.o 

S_UPPER_DEPS += \
./src/start.d 

C_DEPS += \
./src/__main.d \
./src/fmt_8long.d \
./src/fmt_str.d \
./src/fmt_ulongpadright.d \
./src/heap_stack.d \
./src/ls.d \
./src/reloc.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -ffunction-sections -fdata-sections -fno-builtin -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative -Wall  -g -D__dietlibc__ -D__arm__ -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ls\src\dietlibc-0.34" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ls\src\dietlibc-0.34\include" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ls\src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.S src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -ffunction-sections -fdata-sections -fno-builtin -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative -Wall  -g -x assembler-with-cpp -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ls\src\dietlibc-0.34" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_ls\src\dietlibc-0.34\include" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

