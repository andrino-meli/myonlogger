compile: myonen_v2.ino
	arduino-cli compile --fqbn arduino:avr:uno ./

install: ..arduino.avr.uno.hex
	arduino-cli upload --fqbn arduino:avr:uno -p /dev/ttyAMC0 ./

console:
	cu -l /dev/ttyAMC0 -s 9600
