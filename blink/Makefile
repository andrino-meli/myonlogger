#PROGRAMMER ?= usbasp-clone
PROGRAMMER ?= usbasp-clone
AVR = m328p
AVR_GCC = atmega328p
ARDUINO_FLASHER = avrdude -p atmega328p -c arduino -b 115200 -P /dev/ttyUSB0

all: main.hex

%.o: %.c
	/usr/bin/avr-gcc -mmcu=$(AVR_GCC) -std=c99 -Os $< -o $@

%.hex: %.o
	avr-objcopy -O ihex $< $@

.PHONY: flash

flash: main.hex
	avrdude -p $(AVR) -c $(PROGRAMMER) -U flash:w:$<
#	avrdude -p $(AVR) -c $(PROGRAMMER) -U flash:w:$< -U lfuse:w:0xff:m -U hfuse:w:0xde:m -U efuse:w:0x05:m
