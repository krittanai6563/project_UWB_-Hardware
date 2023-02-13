#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include<I2C_Anything.h>
#include<Wire.h>

RF24 radio(8, 7); // CE, CSN
//
//long randomNumber;

const byte address[6] = "00001";

const byte MY_address=8;
float y;

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  Wire.begin(MY_address);
  Serial.begin(9600);
  Wire.onReceive(event2);

//  randomSeed(42.0);  
  
}

void loop() {

//  float randomNumber = random(2.0,5.0);
//  
//  //display the random number on the serial monitor
//  Serial.print("The Random Number is = ");
//  Serial.println(randomNumber);
//  radio.write(&randomNumber, sizeof(randomNumber));
//
//  delay(1000);
}

void event2()
{
  I2C_readAnything(y);
  Serial.print("    " );
  Serial.println(y);
  radio.write(&y, sizeof(y));
  delay(1000);
}
