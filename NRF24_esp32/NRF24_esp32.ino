#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(4, 5); // CE, CSN

const byte address[][6] = {"00001","00002","00003"};


void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, address[0]);
  radio.openReadingPipe(2, address[1]);
  radio.openReadingPipe(3, address[2]);
  radio.setPALevel(RF24_PA_MIN);

}

void loop() {
   radio.startListening();
  if (radio.available()) {
    float text;
    radio.read(&text, sizeof(text));
    Serial.print("ค่าจากบอร์ดที่ 1 ");
    Serial.println(text);

    float text1;
    radio.read(&text1, sizeof(text1));
    Serial.print("ค่าจากบอร์ดที่ 2 ");
    Serial.println(text1);

    float text2;
    radio.read(&text2, sizeof(text2));
    Serial.print("ค่าจากบอร์ดที่ 3 ");
    Serial.println(text2);
  }
  
}
