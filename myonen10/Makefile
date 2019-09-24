PROGRAMMER ?= usbasp-clone
AVR = atmega328p
USBASP_FLASHER = /usr/bin/avrdude -p $(AVR) -c usbasp-clone
ARDUINO_FLASHER = avrdude -p atmega328p -c arduino -b 115200 -P /dev/ttyUSB0
FLASHER = $(USBASP_FLASHER)

SRC_DIR = ./src
SRC2_DIR = ./src/utility
OBJ_DIR = ./obj
VPATH = $(SRC_DIR) $(OBJ_DIR) $(SRC2_DIR)
C_FILES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC2_DIR)/*.c)
CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC2_DIR)/*.cpp)
OBJECTS = $(patsubst %.c,%.o,$(C_FILES)) $(pathsubst %.cpp,%.o,$(CPP_FILES))



all: main.hex

%.o: %.cpp objfiles
	/usr/bin/avr-gcc -mmcu=$(AVR) -I$(SRC_DIR) -I$(SRC2_DIR) -Os $< -o ./obj/$@
#	/usr/bin/avr-gcc -mmcu=$(AVR) -std=c99 -I./libs/core -I./libs/SD/src/ -I./libs:w -Os $< -o $@

%.o: %.cpp objfiles
	/usr/bin/avr-gcc -mmcu=$(AVR) -I$(SRC_DIR) -I$(SRC2_DIR) -Os $< -o ./obj/$@
%.hex: %.o
	avr-objcopy -O ihex $< $@

.PHONY: objfiles
objfiles: $(OBJECTS)
	echo $(VPATH)

.PHONY: flash
flash: main.hex
	$(FLASHER) -U flash:w:$<

.PHONY: fuse
fuse:
	$(FLASHER) -U lfuse:w:0xff:m -U hfuse:w:0xde:m -U efuse:w:0x05:m

.PHONY: clean
clean:
	rm $(OBJECTS)

.PHONY: print
print:
	echo "cpp files: $(CPP_FILES)"
	echo "c files: $(C_FILES)"
	echo "resulting object files: $(OBJECTS)"
