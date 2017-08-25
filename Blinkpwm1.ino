


#include <USI_TWI_Master.h>
#include <Wire.h>
#include <TinyWireM.h>



/* LED Blink, Teensyduino Tutorial #1
   http://www.pjrc.com/teensy/tutorial.html
 
   This example code is in the public domain.
*/

// Teensy 2.0 has the LED on pin 11 USING T2.0 
// Teensy++ 2.0 has the LED on pin 6
// Teensy 3.x / Teensy LC have the LED on pin 13
// for adafruit breakout 32u4

#include <EEPROM.h>

const int ledPin = 7;    // was 7
const int PWMpin = 5;
uint8_t E_data = 0;

// the setup() method runs once, when the sketch starts

void setup() 
{
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(PWMpin, OUTPUT);
  //Serial.begin(9600);

// the loop() methor runs over and over again,
// as long as the board has power
}

void loop() 
{
  digitalWrite(ledPin, HIGH);   // set the LED on
  //digitalWrite(PWMpin, LOW);
  analogWrite(PWMpin, 10);
  delay(1000);                  // wait for a second
  //Serial.println("first loop");
  digitalWrite(ledPin, LOW);    // set the LED off
  //digitalWrite(PWMpin, HIGH);
  analogWrite(PWMpin, 75);
  //Serial.println("second loop");

  
  delay(1000);                  // wait for a second
}

