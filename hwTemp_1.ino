/***
    from
    http://www.narkidae.com/research/atmega-core-temperature-sensor/

    https://www.arduino.cc/en/Tutorial/Button
    
***/
#include <Servo.h>
#include "Adafruit_LiquidCrystal.h"

#define TEMP_OFFSET -7
//const int buttonPin = 5;

const float AREF PROGMEM = 2.56;
const float ADC_FS PROGMEM = 1023; 
const int buttonPin PROGMEM = 1; // for int test
const int ledPin PROGMEM = 6;
const int ServoPWM PROGMEM = 12;


void setupADC()
{
  cli();  //Disable global interrupts

  //ADC Multiplexer Selection Register
  ADMUX = 0;
  ADMUX |= (1 << REFS1);  //Internal 2.56V Voltage Reference with external capacitor on AREF pin
  ADMUX |= (1 << REFS0);  //Internal 2.56V Voltage Reference with external capacitor on AREF pin
  ADMUX |= (0 << MUX4);  //Temperature Sensor - 100111
  ADMUX |= (0 << MUX3);  //Temperature Sensor - 100111
  ADMUX |= (1 << MUX2);  //Temperature Sensor - 100111
  ADMUX |= (1 << MUX1);  //Temperature Sensor - 100111
  ADMUX |= (1 << MUX0);  //Temperature Sensor - 100111

  //ADC Control and Status Register A 
  ADCSRA = 0;
  ADCSRA |= (1 << ADEN);  //Enable the ADC
  ADCSRA |= (1 << ADPS2);  //ADC Prescaler - 16 (16MHz -> 1MHz)

  //ADC Control and Status Register B 
  ADCSRB = 0;
  ADCSRB |= (1 << MUX5);  //Temperature Sensor - 100111

  sei();  //Enable global interrupts
}
/********************
 * 
 * FOR TEMP SENSOR
 * 
 * 
 *************************/
float getTemp()
{
  unsigned int wADC;
  ADCSRA |= (1 << ADSC);  //Start temperature conversion
  while (bit_is_set(ADCSRA, ADSC));  //Wait for conversion to finish
  byte low  = ADCL;
  byte high = ADCH;
  int temperature = (high << 8) | low;  //Result is in kelvin
  // return temperature - 273 + TEMP_OFFSET;

  //float l_temperature = temperature*(AREF/ADC_FS);
  float l_temperature = (wADC -324.1)/1.22;

  Serial.print("ADC Raw: ");
  Serial.println(temperature);
  Serial.print("ADC calc: ");
  Serial.println(l_temperature);
  return temperature;

  //wADC = ADCW;
  //temperature = (wADC -324.1)/1.22;
  //return temperature;
}

Adafruit_LiquidCrystal lcd(0);
Servo myservo1;  
int buttonState = 0; 
int pos =0; 

void setup()
{
  // myservo1.attach(ServoPWM, 1000, 2000);
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  //Start Serial Port
  Serial.begin(9600);
  lcd.begin(20,4);
  lcd.clear();   
  lcd.home(); 
  lcd.print(F("USB/Serial LCD"));
  lcd.setCursor(0,1);   
  lcd.blink();

  setupADC();
  digitalWrite(ledPin, HIGH);
  buttonState=0; 
  //myservo1.write(100); 
  attachInterrupt (digitalPinToInterrupt (buttonPin), buttonRead, CHANGE);  // attach interrupt handler
}

char buffer[20];
char convertbuffer[20];

void loop()
{
  // loopServo();

  
  Serial.print("Time: ");
  Serial.println(millis());
  Serial.print(" Core Temp: ");
  Serial.print(getTemp());
  Serial.println(" C");
  
  sprintf(buffer, "Temp is %i: ", getTemp());
  Serial.println(buffer);
  
  lcd.setCursor(0,1);
  lcd.print(F("Time: "));
  lcd.print(millis());
  lcd.setCursor(0,2);
  lcd.print(F("Core Temp: "));
  lcd.print(getTemp());

  //dtostrf(getTemp(),1, 2, convertbuffer);
  //sprintf(buffer, "Temp is: %s ",convertbuffer);
  //lcd.print(buffer);


  
  
  //delay(1000);
   // buttonRead();
/*****************************  
  if(buttonState == 1)
  {
    buttonState = 0;
    digitalWrite(ledPin, LOW);
    Serial.print(F("Button State: LOW "));
  }
  else
  {
    buttonState = 1;
    digitalWrite(ledPin, HIGH);
    Serial.print(F("Button State: HIGH"));
  }
*****************************/
  delay(1000);

}

bool buttonRead()
{
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) 
  {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    Serial.print(F("Button State: HIGH"));
    return 1;
  } 
  
  else 
  {
    // turn LED off:
    digitalWrite(ledPin, LOW);
    Serial.print(F("Button State: LOW "));
    return 0;
  }
}


void loopServo()
{
  for(pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo1.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo1.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 

}






