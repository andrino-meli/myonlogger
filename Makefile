# -------------- setup programmer variables -------------#
AVR = atmega328p

USBASP_FLASHER = /usr/bin/avrdude -p $(AVR) -c usbasp-clone
ARDUINO_FLASHER = avrdude -p atmega328p -c arduino -b 115200 -P /dev/ttyUSB0

FLASHER = $(USBASP_FLASHER)


# ----------------- setup build variables ---------------#
SRCDIR = src/
BUILDDIR = build/
CLOCKSPEED = 16000000UL
CFLAGS = -Wall -Os -DF_CPU=${CLOCKSPEED}
SHELL = /bin/bash
CC = /usr/bin/avr-gcc -mmcu=${AVR} -I${SRCDIR} ${CFLAGS}

LINKFILES = ${BUILDDIR}SPI.o ${BUILDDIR}Print.o ${BUILDDIR}Stream.o \
${BUILDDIR}LiquidCrystal.o ${BUILDDIR}SD.o ${BUILDDIR}utility/Sd2Card.o \
${BUILDDIR}utility/SdFile.o ${BUILDDIR}utility/SdVolume.o \
${BUILDDIR}HardwareSerial.o ${BUILDDIR}wiring.o ${BUILDDIR}WString.o \
${BUILDDIR}USBCore.o ${BUILDDIR}PluggableUSB.o ${BUILDDIR}File.o \
${BUILDDIR}wiring_digital.o ${BUILDDIR}hooks.o ${BUILDDIR}HardwareSerial0.o \
${BUILDDIR}abi.o
# ------------------------- tasks -----------------------#

all: ${BUILDDIR}main.hex

${BUILDDIR}main.hex: ${BUILDDIR}main.o ; /usr/bin/avr-objcopy -O ihex $< $@

${BUILDDIR}main.o: ${LINKFILES}
	${CC} main.cpp $^ -o $@

${BUILDDIR}%.o: ${SRCDIR}%.cpp
	${CC} $< -c -o $@

${BUILDDIR}%.o: ${SRCDIR}%.c
	${CC} $< -c -o $@


${BUILDDIR}utility/%.o: ${SRCDIR}utility/%.cpp
	-mkdir ${BUILDDIR}/utility/
	${CC} $< -c -o $@




# ------------------------- utils -----------------------#

.PHONY: flash
flash: main.hex
	$(FLASHER) -U flash:w:$<

.PHONY: fuse
fuse:
	$(FLASHER) -U lfuse:w:0xff:m -U hfuse:w:0xde:m -U efuse:w:0x05:m

.PHONY: clean
clean:
	rm -r ${BUILDDIR}*
