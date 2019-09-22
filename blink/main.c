#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz clock speed
#endif

#include <avr/io.h>
#include <util/delay.h>

#define set_bit(byte,bit)		(byte |= ( 1 << bit ))
#define clear_bit(byte,bit)		(byte &= ~( 1 << bit ))
#define toggle_bit(byte,bit)	(byte ^= ( 1 << bit ))

#define LED PB5

int main(void)
{
	set_bit(DDRB,LED);
	set_bit(PORTB,LED);

	while(1) {
		toggle_bit(PORTB,LED);
		_delay_ms(1000); //1 second delay
	}
}
