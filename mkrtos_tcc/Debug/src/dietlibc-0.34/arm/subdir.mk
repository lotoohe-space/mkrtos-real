################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/dietlibc-0.34/arm/__fadvise.c 

S_UPPER_SRCS += \
../src/dietlibc-0.34/arm/__aeabi_read_tp.S \
../src/dietlibc-0.34/arm/__aeabi_unwind_cpp.S \
../src/dietlibc-0.34/arm/__testandset.S \
../src/dietlibc-0.34/arm/arm_fadvise.S \
../src/dietlibc-0.34/arm/arm_set_tls.S \
../src/dietlibc-0.34/arm/dyn_syscalls.S \
../src/dietlibc-0.34/arm/mcount.S \
../src/dietlibc-0.34/arm/md5asm.S \
../src/dietlibc-0.34/arm/mmap.S \
../src/dietlibc-0.34/arm/setsockopt.S \
../src/dietlibc-0.34/arm/start.S 

OBJS += \
./src/dietlibc-0.34/arm/__aeabi_read_tp.o \
./src/dietlibc-0.34/arm/__aeabi_unwind_cpp.o \
./src/dietlibc-0.34/arm/__fadvise.o \
./src/dietlibc-0.34/arm/__testandset.o \
./src/dietlibc-0.34/arm/arm_fadvise.o \
./src/dietlibc-0.34/arm/arm_set_tls.o \
./src/dietlibc-0.34/arm/dyn_syscalls.o \
./src/dietlibc-0.34/arm/mcount.o \
./src/dietlibc-0.34/arm/md5asm.o \
./src/dietlibc-0.34/arm/mmap.o \
./src/dietlibc-0.34/arm/setsockopt.o \
./src/dietlibc-0.34/arm/start.o 

S_UPPER_DEPS += \
./src/dietlibc-0.34/arm/__aeabi_read_tp.d \
./src/dietlibc-0.34/arm/__aeabi_unwind_cpp.d \
./src/dietlibc-0.34/arm/__testandset.d \
./src/dietlibc-0.34/arm/arm_fadvise.d \
./src/dietlibc-0.34/arm/arm_set_tls.d \
./src/dietlibc-0.34/arm/dyn_syscalls.d \
./src/dietlibc-0.34/arm/mcount.d \
./src/dietlibc-0.34/arm/md5asm.d \
./src/dietlibc-0.34/arm/mmap.d \
./src/dietlibc-0.34/arm/setsockopt.d \
./src/dietlibc-0.34/arm/start.d 

C_DEPS += \
./src/dietlibc-0.34/arm/__fadvise.d 


# Each subdirectory must supply rules for building sources it contributes
src/dietlibc-0.34/arm/%.o: ../src/dietlibc-0.34/arm/%.S src/dietlibc-0.34/arm/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -ffunction-sections -fdata-sections -fno-builtin -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative -Wl,--gc-sections  -fdata-sections -ffunction-sections -Wall  -g -x assembler-with-cpp -I"D:\Project\MyProject1\mkrtos_real\mkrtos_tcc\src\dietlibc-0.34" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_tcc\src\dietlibc-0.34\include" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/dietlibc-0.34/arm/%.o: ../src/dietlibc-0.34/arm/%.c src/dietlibc-0.34/arm/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Os -ffunction-sections -fdata-sections -fno-builtin -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative -Wl,--gc-sections  -fdata-sections -ffunction-sections -Wall  -g -D__dietlibc__ -D__arm__ -I"D:\Project\MyProject1\mkrtos_real\mkrtos_tcc\src\tcc" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_tcc\src\dietlibc-0.34" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_tcc\src\dietlibc-0.34\include" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_tcc\src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


