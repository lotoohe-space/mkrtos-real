################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/dietlibc-0.34/libcompat/clearenv.c \
../src/dietlibc-0.34/libcompat/ctermid.c \
../src/dietlibc-0.34/libcompat/cuserid.c \
../src/dietlibc-0.34/libcompat/daemon.c \
../src/dietlibc-0.34/libcompat/err.c \
../src/dietlibc-0.34/libcompat/errx.c \
../src/dietlibc-0.34/libcompat/ffsl.c \
../src/dietlibc-0.34/libcompat/ffsll.c \
../src/dietlibc-0.34/libcompat/ftok.c \
../src/dietlibc-0.34/libcompat/getdelim.c \
../src/dietlibc-0.34/libcompat/getline.c \
../src/dietlibc-0.34/libcompat/getutmp.c \
../src/dietlibc-0.34/libcompat/mempcpy.c \
../src/dietlibc-0.34/libcompat/rcmd.c \
../src/dietlibc-0.34/libcompat/re_bsd.c \
../src/dietlibc-0.34/libcompat/toascii.c \
../src/dietlibc-0.34/libcompat/utmpxname.c \
../src/dietlibc-0.34/libcompat/verr.c \
../src/dietlibc-0.34/libcompat/verrx.c \
../src/dietlibc-0.34/libcompat/vwarn.c \
../src/dietlibc-0.34/libcompat/vwarnx.c \
../src/dietlibc-0.34/libcompat/warn.c \
../src/dietlibc-0.34/libcompat/warnx.c \
../src/dietlibc-0.34/libcompat/wtxent.c 

S_UPPER_SRCS += \
../src/dietlibc-0.34/libcompat/syscall.S 

OBJS += \
./src/dietlibc-0.34/libcompat/clearenv.o \
./src/dietlibc-0.34/libcompat/ctermid.o \
./src/dietlibc-0.34/libcompat/cuserid.o \
./src/dietlibc-0.34/libcompat/daemon.o \
./src/dietlibc-0.34/libcompat/err.o \
./src/dietlibc-0.34/libcompat/errx.o \
./src/dietlibc-0.34/libcompat/ffsl.o \
./src/dietlibc-0.34/libcompat/ffsll.o \
./src/dietlibc-0.34/libcompat/ftok.o \
./src/dietlibc-0.34/libcompat/getdelim.o \
./src/dietlibc-0.34/libcompat/getline.o \
./src/dietlibc-0.34/libcompat/getutmp.o \
./src/dietlibc-0.34/libcompat/mempcpy.o \
./src/dietlibc-0.34/libcompat/rcmd.o \
./src/dietlibc-0.34/libcompat/re_bsd.o \
./src/dietlibc-0.34/libcompat/syscall.o \
./src/dietlibc-0.34/libcompat/toascii.o \
./src/dietlibc-0.34/libcompat/utmpxname.o \
./src/dietlibc-0.34/libcompat/verr.o \
./src/dietlibc-0.34/libcompat/verrx.o \
./src/dietlibc-0.34/libcompat/vwarn.o \
./src/dietlibc-0.34/libcompat/vwarnx.o \
./src/dietlibc-0.34/libcompat/warn.o \
./src/dietlibc-0.34/libcompat/warnx.o \
./src/dietlibc-0.34/libcompat/wtxent.o 

S_UPPER_DEPS += \
./src/dietlibc-0.34/libcompat/syscall.d 

C_DEPS += \
./src/dietlibc-0.34/libcompat/clearenv.d \
./src/dietlibc-0.34/libcompat/ctermid.d \
./src/dietlibc-0.34/libcompat/cuserid.d \
./src/dietlibc-0.34/libcompat/daemon.d \
./src/dietlibc-0.34/libcompat/err.d \
./src/dietlibc-0.34/libcompat/errx.d \
./src/dietlibc-0.34/libcompat/ffsl.d \
./src/dietlibc-0.34/libcompat/ffsll.d \
./src/dietlibc-0.34/libcompat/ftok.d \
./src/dietlibc-0.34/libcompat/getdelim.d \
./src/dietlibc-0.34/libcompat/getline.d \
./src/dietlibc-0.34/libcompat/getutmp.d \
./src/dietlibc-0.34/libcompat/mempcpy.d \
./src/dietlibc-0.34/libcompat/rcmd.d \
./src/dietlibc-0.34/libcompat/re_bsd.d \
./src/dietlibc-0.34/libcompat/toascii.d \
./src/dietlibc-0.34/libcompat/utmpxname.d \
./src/dietlibc-0.34/libcompat/verr.d \
./src/dietlibc-0.34/libcompat/verrx.d \
./src/dietlibc-0.34/libcompat/vwarn.d \
./src/dietlibc-0.34/libcompat/vwarnx.d \
./src/dietlibc-0.34/libcompat/warn.d \
./src/dietlibc-0.34/libcompat/warnx.d \
./src/dietlibc-0.34/libcompat/wtxent.d 


# Each subdirectory must supply rules for building sources it contributes
src/dietlibc-0.34/libcompat/%.o: ../src/dietlibc-0.34/libcompat/%.c src/dietlibc-0.34/libcompat/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -ffunction-sections -fdata-sections -fno-builtin -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative -Wall  -g -D__dietlibc__ -D__arm__ -I"D:\Project\MyProject1\mkrtos_real\mkrtos_app\src\dietlibc-0.34" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_app\src\dietlibc-0.34\include" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_app\src" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/dietlibc-0.34/libcompat/%.o: ../src/dietlibc-0.34/libcompat/%.S src/dietlibc-0.34/libcompat/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -ffunction-sections -fdata-sections -fno-builtin -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative -Wall  -g -x assembler-with-cpp -I"D:\Project\MyProject1\mkrtos_real\mkrtos_app\src\dietlibc-0.34" -I"D:\Project\MyProject1\mkrtos_real\mkrtos_app\src\dietlibc-0.34\include" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


