################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Sources/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
dmx_adapter.lss \

FLASH_IMAGE += \
dmx_adapter.hex \

EEPROM_IMAGE += \
dmx_adapter.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: dmx_adapter.elf secondary-outputs

# Tool invocations
dmx_adapter.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,dmx_adapter.map -mmcu=atmega8 -o "dmx_adapter.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

dmx_adapter.lss: dmx_adapter.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S dmx_adapter.elf  >"dmx_adapter.lss"
	@echo 'Finished building: $@'
	@echo ' '

dmx_adapter.hex: dmx_adapter.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex dmx_adapter.elf  "dmx_adapter.hex"
	@echo 'Finished building: $@'
	@echo ' '

dmx_adapter.eep: dmx_adapter.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex dmx_adapter.elf  "dmx_adapter.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: dmx_adapter.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega8 dmx_adapter.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) dmx_adapter.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
