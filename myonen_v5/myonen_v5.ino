#include <SD.h>

//edit the next line to toggle debuging
#define DEBUG
#ifdef DEBUG
  #define DEBUG_PRINT(x) Serial.println(x);
#else
  #define DEBUG_PRINT(x)  
#endif

#define SD_PIN 10
#define TERMINATE_PIN 3
#define LED_PIN 4
volatile uint32_t counts;

//if the termination pin gets wired low close the file and stop
bool shouldTerminate(){
  if(!digitalRead(TERMINATE_PIN)){
    delay(100); //unprell the input
    if(!digitalRead(TERMINATE_PIN))
      return false;
  }
  return true;
}

void setup()
{
  //setup external interupts on pin 0 (D2)
  DDRD &= ~(1 << DDD2);  //set pin D2 as input (0)
  EIMSK |= (1 << INT0);
  EICRA |= (1 << ISC01);  //set controll to "falling edge"
  EICRA &= ~(1 << ISC00);
  sei(); //enable interrupts

  //set up pin "chip select" for SPI communication
  Serial.begin(9600);
  pinMode(SD_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  //set up pin for start/stop
  pinMode(TERMINATE_PIN,INPUT_PULLUP);

  bool flag = false;
  while(!SD.begin(SD_PIN)){ // loop necessary as sometimes the init process bugs..
    DEBUG_PRINT("SD init failed!");
    if(flag) digitalWrite(LED_PIN, HIGH);
    else digitalWrite(LED_PIN, LOW);
    flag = !flag;
  }
  
  if (true){
    DEBUG_PRINT("SD init successfully!")
  }
  
  //resolve and open the log file
  const String prefix ="log_"; //change to your needs
  const String suffix = ".my";
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
  while(shouldTerminate()){
     DEBUG_PRINT("Reconnect the wire to start measurements")
     delay(1000);
  }
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  DEBUG_PRINT("start measurement.....")
  
  uint16_t last_counts = 0;
  uint16_t counts_buffer = 0;
  uint32_t time_buffer = 0;
  uint8_t timeMSB;
  uint8_t timeNMSB;
  uint8_t timeNLSB;
  uint8_t timeLSB;
  uint8_t countsMSB;
  uint8_t countsLSB;
  while(true) {
    for (int i = 0; i < 256; i++){
      //cli();
      counts_buffer = counts; //TODO check if this solution is nice and how many clock cycle it uses
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
      DEBUG_PRINT("Closing file" + filename +" and shuting down!")
      file.write(-1);
      file.close();
      digitalWrite(LED_PIN, HIGH);
      break;
    }
     
  }
}


void loop(){
  DEBUG_PRINT("idle..press restart pin to restart")
  delay(30000);
}

//interrupt service routine
ISR (INT0_vect){
  counts++;
}
