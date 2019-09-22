PROGRAMMER ?= usbasp-clone
AVR = atmega328p

USBASP_FLASHER = /usr/bin/avrdude -p $(AVR) -c usbasp-clone
ARDUINO_FLASHER = avrdude -p atmega328p -c arduino -b 115200 -P /dev/ttyUSB0
FLASHER = $(ARDUINO_FLASHER)


all: main.hex

clean:
	

%.o: %.c
	/usr/bin/avr-gcc -mmcu=$(AVR) -std=c99 -Os $< -o $@

%.hex: %.o
	avr-objcopy -O ihex $< $@

.PHONY: flash
flash: main.hex
	$(FLASHER) -U flash:w:$<

.PHONY: fuse
fuse:
	$(FLASHER) -U lfuse:w:0xff:m -U hfuse:w:0xde:m -U efuse:w:0x05:m
