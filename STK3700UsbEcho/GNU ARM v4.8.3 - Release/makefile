################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include emlib/subdir.mk
-include drivers/usb/emusb/subdir.mk
-include drivers/usb/subdir.mk
-include drivers/subdir.mk
-include CMSIS/efm32gg/subdir.mk
-include BSP/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: STK3700_usbEcho.axf

# Tool invocations
STK3700_usbEcho.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m3 -mthumb -T "STK3700_usbEcho.ld" -Xlinker --gc-sections -Xlinker -Map="STK3700_usbEcho.map" --specs=nano.specs -o STK3700_usbEcho.axf "./src/main.o" "./src/utils.o" "./emlib/em_assert.o" "./emlib/em_cmu.o" "./emlib/em_dma.o" "./emlib/em_ebi.o" "./emlib/em_emu.o" "./emlib/em_gpio.o" "./emlib/em_int.o" "./emlib/em_lcd.o" "./emlib/em_msc.o" "./emlib/em_system.o" "./emlib/em_timer.o" "./emlib/em_usart.o" "./drivers/usb/emusb/em_usbd.o" "./drivers/usb/emusb/em_usbdch9.o" "./drivers/usb/emusb/em_usbdep.o" "./drivers/usb/emusb/em_usbdint.o" "./drivers/usb/emusb/em_usbhal.o" "./drivers/usb/emusb/em_usbtimer.o" "./drivers/usb/cdc.o" "./drivers/usb/descriptors.o" "./drivers/ethspi.o" "./drivers/gpiointerrupt.o" "./drivers/segmentlcd.o" "./drivers/sleep.o" "./CMSIS/efm32gg/startup_gcc_efm32gg.o" "./CMSIS/efm32gg/system_efm32gg.o" "./BSP/bsp_bcc.o" "./BSP/bsp_stk.o" "./BSP/bsp_stk_leds.o" "./BSP/bsp_trace.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: STK3700_usbEcho.hex'
	arm-none-eabi-objcopy -O ihex "STK3700_usbEcho.axf" "STK3700_usbEcho.hex"
	@echo ' '

	@echo 'Building bin file: STK3700_usbEcho.bin'
	arm-none-eabi-objcopy -O binary "STK3700_usbEcho.axf" "STK3700_usbEcho.bin"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "STK3700_usbEcho.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) STK3700_usbEcho.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
