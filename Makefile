# Makefile for DC Motor Control System - Dual Atmega32

# Compiler and tools
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size

# MCU configuration
MCU = atmega32
F_CPU = 16000000UL

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra
CFLAGS += -std=gnu99 -ffunction-sections -fdata-sections
LDFLAGS = -mmcu=$(MCU) -Wl,--gc-sections

# Directories
SRC_DIR = src
BUILD_DIR = build
COMMON_DIR = $(SRC_DIR)/common
MCU1_DIR = $(SRC_DIR)/mcu1
MCU2_DIR = $(SRC_DIR)/mcu2

# Common sources
COMMON_SRC = $(COMMON_DIR)/lcd.c

# MCU1 sources
MCU1_SRC = $(MCU1_DIR)/main.c \
           $(MCU1_DIR)/i2c_master.c \
           $(MCU1_DIR)/keypad.c \
           $(COMMON_SRC)

# MCU2 sources
MCU2_SRC = $(MCU2_DIR)/main.c \
           $(MCU2_DIR)/i2c_slave.c \
           $(MCU2_DIR)/motor.c \
           $(MCU2_DIR)/encoder.c \
           $(MCU2_DIR)/pid.c \
           $(COMMON_SRC)

# Output files
MCU1_TARGET = mcu1
MCU2_TARGET = mcu2

# Build directories
MCU1_BUILD = $(BUILD_DIR)/mcu1
MCU2_BUILD = $(BUILD_DIR)/mcu2

# Default target
all: mcu1 mcu2

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(MCU1_BUILD):
	mkdir -p $(MCU1_BUILD)

$(MCU2_BUILD):
	mkdir -p $(MCU2_BUILD)

# MCU1 build
mcu1: $(MCU1_BUILD) $(MCU1_BUILD)/$(MCU1_TARGET).hex
	@echo "MCU1 build complete!"
	@$(SIZE) $(MCU1_BUILD)/$(MCU1_TARGET).elf

$(MCU1_BUILD)/$(MCU1_TARGET).elf: $(MCU1_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(MCU1_BUILD)/$(MCU1_TARGET).hex: $(MCU1_BUILD)/$(MCU1_TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(OBJDUMP) -h -S $< > $(MCU1_BUILD)/$(MCU1_TARGET).lst

# MCU2 build
mcu2: $(MCU2_BUILD) $(MCU2_BUILD)/$(MCU2_TARGET).hex
	@echo "MCU2 build complete!"
	@$(SIZE) $(MCU2_BUILD)/$(MCU2_TARGET).elf

$(MCU2_BUILD)/$(MCU2_TARGET).elf: $(MCU2_SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(MCU2_BUILD)/$(MCU2_TARGET).hex: $(MCU2_BUILD)/$(MCU2_TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(OBJDUMP) -h -S $< > $(MCU2_BUILD)/$(MCU2_TARGET).lst

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Program MCU1 using avrdude (adjust programmer and port as needed)
flash-mcu1: mcu1
	avrdude -p $(MCU) -c usbasp -U flash:w:$(MCU1_BUILD)/$(MCU1_TARGET).hex:i

# Program MCU2 using avrdude (adjust programmer and port as needed)
flash-mcu2: mcu2
	avrdude -p $(MCU) -c usbasp -U flash:w:$(MCU2_BUILD)/$(MCU2_TARGET).hex:i

# Flash both MCUs
flash: flash-mcu1 flash-mcu2

.PHONY: all clean mcu1 mcu2 flash flash-mcu1 flash-mcu2
