#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(5, 4); // CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    float text;
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }
}
