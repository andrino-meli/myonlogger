volatile unsigned int count = 0;
unsigned int last_count = 0;

const int sd = 10;
const String file_number = "1";
const char filename[20];


void setup() {
  Serial.begin(9600);
  pinMode(sd,OUTPUT);
  
  //setup external interupts on pin 0 (D2)
  DDRD &= ~(1 << DDD2);  //set pin D2 as input (0)
  PORTD |= (1 << PORTD2);   //turn on pullup
  EIMSK |= (1 << INT0);
  EICRA |= (1 << ISC01);  //set controll to "falling edge"
  EICRA &= ~(1 << ISC00);
  sei(); //enable interrupts
}

ISR (INT0_vect){
  count++;
}

void loop() {
    Serial.println(String(count-last_count));
    last_count = count;
    delay(10);
}
