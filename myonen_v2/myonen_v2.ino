#include <SD.h>
//#define F_CPU 16000000UL //16MHz Clock
//#include <avr/io.h>
//#include <util/delay.h>
//#include <avr/interrupt.h>


#undef DEBUG //set or unset (#define or #undef) compile time debug Flag
#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.println(x);
#else
  #define DEBUG_PRINT(x)
#endif

const int sd = 10;
const String file_number = "3";
const char filename[20];

const unsigned char buffer_size = 256; //convinient because then the read and write index allmost overflow automatically
volatile unsigned long myon_buffer[buffer_size];
volatile unsigned char read_index = 0; // same as below but indicates the progress 
volatile unsigned char write_index = 0; //(write_index % buffer_size) points to te next empty space in the buffer
volatile bool buffer_dump_occured = false; //a buffer dump occures if write index gets to read index
// and therefor overwrites not yes logged data, in this case the entire buffer gets dumped and 256 measurements get lost
// we prefere rare and bruttal dumps over 

unsigned long counts = 0;      //Counts since start

void setup()
{
  //setup external interupts on pin 0 (D2)
  DDRD &= ~(1 << DDD2);  //set pin D2 as input (0)
  PORTD |= (1 << PORTD2);   //turn on pullup
  EIMSK |= (1 << INT0);
  EICRA |= (1 << ISC01);  //set controll to "falling edge"
  EICRA &= ~(1 << ISC00);
  sei(); //enable interrupts

  pinMode(sd,OUTPUT);
  
  //initialize SD card and check for fail
  bool succes = SD.begin(sd);
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("Debug Mode:");
    if(!succes){
      Serial.println("SD-Card failed, abort.");
      while(true) delay(10000);
    }
    else{
      Serial.println("SD-Card succeded.");
    }
  #else
    if(!succes){
      while(true) delay(10000);
    }
  #endif

  
   //calculate file-name
  const String file_head = "test";
  const String file_tail = ".txt";
  String filestring = file_head + file_number + file_tail;
  filestring.toCharArray(filename, filestring.length() + 1);
  //std::strcopy(filename, filestring.c_str());
}

//interrupt service routine
ISR (INT0_vect){
  myon_buffer[write_index] = millis();
  write_index++;
  if(write_index == read_index)
    buffer_dump_occured = true;
  delay(1);
}

void loop()
{
    File file = SD.open(filename,FILE_WRITE);
    
    for(int i = 0; i < 32; i++){
      if(buffer_dump_occured){
        DEBUG_PRINT("Buffer dumped");
        file.println("-1");
        buffer_dump_occured = false;
      }
      while(read_index != write_index){
        file.println(myon_buffer[read_index]);
        read_index++;
        counts++;
      }
  }
  file.close();
  DEBUG_PRINT(counts);
}
