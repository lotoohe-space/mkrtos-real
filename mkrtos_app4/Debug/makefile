################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/dietlibc-0.34/profiling/subdir.mk
-include src/dietlibc-0.34/libstdio/subdir.mk
-include src/dietlibc-0.34/libshell/subdir.mk
-include src/dietlibc-0.34/libm/subdir.mk
-include src/dietlibc-0.34/libcompat/subdir.mk
-include src/dietlibc-0.34/lib/subdir.mk
-include src/dietlibc-0.34/arm/subdir.mk
-include src/dietlibc-0.34/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := mkrtos_app4
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
mkrtos_app4.hex \

SECONDARY_SIZE += \
mkrtos_app4.siz \


# All Target
all: mkrtos_app4.elf secondary-outputs

# Tool invocations
mkrtos_app4.elf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU Arm Cross C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -ffunction-sections -fdata-sections -fno-builtin -fPIC -n -pie -fpie -fPIC -fpic -msingle-pic-base -mno-pic-data-is-text-relative -Wall  -g -T "D:\Project\MyProject1\mkrtos_real\mkrtos_app4\link.lds" -nostartfiles -nodefaultlibs -nostdlib -Xlinker --gc-sections -L"D:\Project\MyProject1\mkrtos_real\mkrtos_sdk\Debug" -Wl,-Map,"mkrtos_app4.map" -Xlinker --cref -o "mkrtos_app4.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

mkrtos_app4.hex: mkrtos_app4.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GNU Arm Cross Create Flash Image'
	arm-none-eabi-objcopy -O ihex "mkrtos_app4.elf"  "mkrtos_app4.hex"
	@echo 'Finished building: $@'
	@echo ' '

mkrtos_app4.siz: mkrtos_app4.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GNU Arm Cross Print Size'
	arm-none-eabi-size --format=berkeley "mkrtos_app4.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) mkrtos_app4.elf
	-@echo ' '

post-build:
	-arm-none-eabi-objcopy -O binary "mkrtos_app4.elf" "app.bi" && arm-none-eabi-strip "mkrtos_app4.elf" -o "app.elf"
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents post-build

-include ../makefile.targets
