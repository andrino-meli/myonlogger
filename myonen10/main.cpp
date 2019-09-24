//#include <avr/io.h>
//#include <util/delay.h>
#include "Arduino.h"
#include "SD.h"
#include "LiquidCrystal.h"

#ifndef F_CPU
	#define F_CPU 16000000UL //16MHz clock speed
#endif

//edit the next line to toggle debuging
#define DEBUG
#ifdef DEBUG
	#define DEBUG_PRINT(x) Serial.println(x);
#else
	#define DEBUG_PRINT(x)  
#endif

// ============================== PINOUT =======================================
// ! The here defined pins are from the Atmega328P package pinout not from the 
//   Arduino IDE codes !
//pin 1 for reset
//pin 2 and 3 for serial communication
//pin 4 is the INT0 pin for the muon counts input
#define MUON_PIN 2
//pin 5 is the INT1 pin for the LCD change button
#define LCD_PIN 3
//pin 6 is the input for the Timer 0 and here use to enable the measurement


//pin 7 is Vcc
//pin 8 GND
//pin 9 and
//pin 10 are for the cristal osz.
//pin 11 is Timer 1

//pin 12

//pin 13
#define LCD_RS_PIN 7
//pin 14
#define LCD_ENABLE_PIN 8
//pin 15
#define LED_PIN 9
//pin 16 slave select for SPI bus
#define SD_PIN 10
//pin 17 MOSI for SPI
//pin 18 MISO for SPI
//pin 19 SCK clock for SPI
//pin 20 is VCC
//pin 21 is AREF
//pin 22 is GND
//pin 23 upto 26 are for the lcd data bits 7 down to 4
#define DATA7_PIN A0
#define DATA6_PIN A1
#define DATA5_PIN A2
#define DATA4_PIN A3
//pin 27 is SDA for I2C and not used
#define PADDLE1_PIN 18
//pin 28 is SCL for I2C and not used
#define PADDLE2_PIN 19


#define TERMINATE_PIN 12 //TODO remove
bool shouldTerminate(){ return true;} //TODO remove


//========================== globs =============================================
volatile uint32_t counts = 0;
volatile uint32_t paddle1_counts = 0;
volatile uint32_t paddle2_counts = 0;
volatile uint8_t lcd_control = 0;

//========================= functions ==========================================
//if the termination pin gets wired low close the file and stop
//bool shouldTerminate(){
//	if(!digitalRead(TERMINATE_PIN)){
//		delay(100); //unprell the input
//		if(!digitalRead(TERMINATE_PIN))
//			return false;
//	}
//	return true;
//}


//resizes an input String to contain atleast 16 characters
String resize(String in){
	if(in.length() > 16) return in;

	char out[17] = "                ";
	for(int i = 0; i < in.length(); i++){
		out[i] = in[i];
	}
	return out;
}

void lcdprint(LiquidCrystal& lcd, String top, String bot){
	lcd.setCursor(0,0);
	lcd.print(resize(top));
	lcd.setCursor(0,1);
	lcd.print(resize(bot));
}

void lcdprint(LiquidCrystal& lcd, String top){
	lcdprint(lcd,top,"");
}

ISR( INT0_vect ){
	counts++;
}

ISR( INT1_vect ){
	lcd_control++;
}

ISR( TIMER0_OVF_vect ) {
	counts++;
}
//========================= setup ==============================================
int main()
{
	//setup external interupts on pin 4 (D2)
	DDRD &= ~(1 << DDD2); //set pin D2 as input (0)
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01); //set controll to "rising edge"
	EICRA &= ~(1 << ISC00);
	//setup external interrupts on pin 5 (D3)
	DDRD &= ~(1 << DDD3); //set pin D3 as input
	EIMSK |= (1 << INT1);
	EICRA |= (1 << ISC11); //set controll to "rising edge"
	EICRA |= (1 << ISC10);

	//set up pin "chip select" for SPI communication
#ifdef DEBUG
	Serial.begin(9600);
	DEBUG_PRINT("Reset Microcontroller, starting setup")
#endif

	pinMode(SD_PIN, OUTPUT);
	pinMode(LED_PIN, OUTPUT);
	pinMode(TERMINATE_PIN,INPUT);
	LiquidCrystal lcd(LCD_RS_PIN, LCD_ENABLE_PIN, DATA4_PIN, DATA5_PIN, DATA6_PIN, DATA7_PIN);
	lcd.begin(16,2);
	DEBUG_PRINT("Init Liquid Crystal LCD")

	while(!SD.begin(SD_PIN)){ // loop necessary as sometimes the init process bugs
		DEBUG_PRINT("SD init failed!")
		lcdprint(lcd, "SD init failed!");
		digitalWrite(LED_PIN, HIGH);
	}
	DEBUG_PRINT("SD init successfully!")
	digitalWrite(LED_PIN,LOW);

	//resolve and open the log file
	const String prefix ="log_"; //change to your needs
	const String suffix = ".bin";
	String filename;
	for(unsigned int i = 0; true; i++){
		String tmp = prefix + String(i) + suffix;
		DEBUG_PRINT("try logg to " + String(i))
		if(!SD.exists(tmp)){
			filename = tmp;
			break;
		}
	}
	DEBUG_PRINT("loging to file: " + filename)
	File file = SD.open(filename,FILE_WRITE);


	digitalWrite(LED_PIN, HIGH);
	if(shouldTerminate){
		 DEBUG_PRINT("Reconnect the wire to start measurements")
		 lcdprint(lcd, "start measuremnt","with the switch");
	}
	while(shouldTerminate()){
		 delay(100);
	}
	digitalWrite(LED_PIN, LOW);
	DEBUG_PRINT("start measurement.....")
	lcdprint(lcd, "Measuring...", "don't interrupt"); //TODO maybee show status but do it efficent
  
	uint16_t last_counts = 0;
	uint16_t counts_buffer = 0;
	uint32_t time_buffer = 0;
	uint8_t timeMSB;
	uint8_t timeNMSB;
	uint8_t timeNLSB;
	uint8_t timeLSB;
	uint8_t countsMSB;
	uint8_t countsLSB;
	sei(); //enable interrupts
	bool doTerminate = false;
	while(!doTerminate) {
		//log 256 cyckes and then flush the file
		for (int i = 0; i < 256; i++){
			//cli();
			counts_buffer = counts; //TODO check if this solution is nice and how many clock cycle it uses
			//sei();
			
			if(counts_buffer - last_counts > 0){
				time_buffer = millis();
				//DEBUG_PRINT(String(counts_buffer) + " " + String(counts_buffer - last_counts))
				DEBUG_PRINT(String(time_buffer) + " " + String(counts_buffer))

				timeMSB = time_buffer >> 24;
				timeNMSB = time_buffer >> 16;
				timeNLSB = time_buffer >> 8;
				timeLSB = time_buffer;
				file.write(timeLSB);
				file.write(timeNLSB);
				file.write(timeNMSB);
				file.write(timeMSB); 
				//DEBUG_PRINT(timeMSB);
				//DEBUG_PRINT(timeNMSB)
				//DEBUG_PRINT(timeNLSB)
				//DEBUG_PRINT(timeLSB);

				countsMSB = counts_buffer >> 8;
				countsLSB = counts_buffer;
				file.write(countsLSB);
				file.write(countsMSB);
				//DEBUG_PRINT(countsMSB);
				//DEBUG_PRINT(countsLSB);
				
				last_counts = counts_buffer;
			}
		}
		
		file.flush();
		
		//if the termination pin gets pulled up the system terminates
		if(shouldTerminate()){
			DEBUG_PRINT("Closing file " + filename +" and shuting down!")
			file.write(-1);
			file.close();
			digitalWrite(LED_PIN, HIGH);
			lcdprint(lcd,"Logfile saved to", "> " + filename);
			doTerminate = true;
			delay(5000);
		}
	}

	volatile uint8_t last_lcd_control = 0;
	DEBUG_PRINT("Entering lcd_control")
	while (true){
#ifdef DEBUG
		delay(1000);
#endif
		DEBUG_PRINT(lcd_control)
		//if nothingchanged we don't do anything
		if(last_lcd_control != lcd_control) {
			DEBUG_PRINT("lcd_control changed to " + String(lcd_control) + " modolo is: " + String(lcd_control % 4))
			switch(lcd_control % 4){
				case 0:
					lcdprint(lcd,"Muons: ", String(counts));
					break;
				case 1:
					lcdprint(lcd,"Paddle1: ", String(paddle1_counts));
					break;
				case 2:
					lcdprint(lcd,"Paddle2: ", String(paddle2_counts));
					break;
				case 3:
					lcdprint(lcd,"Logfile: ",filename);
					break;
				default:
					lcdprint(lcd,"Error........");
					break;
			}
			last_lcd_control =lcd_control;
		}
	}
}
