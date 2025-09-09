target ?= atmega328p

COMPILER_CONFIG = -std=gnu++20 -fms-extensions -Wall -Os -Wno-volatile -fno-exceptions -ffunction-sections -fdata-sections -w -fuse-linker-plugin

COMPILER := avr-gcc

define compile-for
	@$(COMPILER) -mmcu=$(strip $(1)) -o output/$(strip $(1))/bin.o $(COMPILER_CONFIG) -DMPU_$(strip $(1)) src/main.cpp
	@avr-objcopy -j .text -j .data -O ihex -R .eeprom output/$(strip $(1))/bin.o output/$(strip $(1))/bin.hex
	@avr-size -A output/$(strip $(1))/bin.o
endef

define upload-for
	@avrdude -v -c arduino -P $(strip $(2)) -p $(strip $(1)) -U flash:w:"output/$(strip $(1))/bin.hex":i -b 115200
endef

flash: compile upload

pre:
	@mkdir -p output/$(target)

compile: pre
	$(call compile-for, $(target))

upload:
	$(call upload-for, $(target), $(port))
