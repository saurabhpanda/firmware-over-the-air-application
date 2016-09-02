###############################################################################
# Makefile for the project cc1101
###############################################################################

## General Flags
MCU = atmega328p
TARGET = out.elf
CC = avr-gcc

##Flags for programmer
TARGET_DEVICE = m328p
PROGRAMMER = usbasp
PORT = usb
MEMORY_TYPE = flash
FILE = out.hex

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=8000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-Map=out.map
LDFLAGS += -Wl,-section-start=.text=0x7000        ##word adress is 0x0000
LDFLAGS += -Wl,-section-start=.stack=0x6400

## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I.  

## Objects that must be built in order to link
OBJECTS =spi_MCU.o radio_private.o radio_cc1101_commands.o bootloader.o   

## Objects explicitly added by the user
LINKONLYOBJECTS = 

LIBS = ##libradio.a

## Build
all: $(TARGET) out.hex out.eep out.lss
uart.o: uart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

spi_MCU.o: spi_MCU.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

radio_private.o: radio_private.c 
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

radio_cc1101_commands.o: radio_cc1101_commands.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<
 
bootloader.o: bootloader.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $< 

##Link -T linker_script.x 
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) -T linker_script.x $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) out.* dep NUL *.eep *.lss *.hex 

program:
	avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -P $(PORT) -U $(MEMORY_TYPE):w:$(FILE)

terminal:
	avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -P $(PORT) -t

## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

