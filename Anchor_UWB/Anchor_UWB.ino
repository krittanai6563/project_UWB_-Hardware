#include<I2C_Anything.h>
#include<Wire.h>

#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"


const byte slave_address_one = 8;
float pa,f;
long dataLen;

 
const uint8_t PIN_RST = PB12; // reset pin
const uint8_t PIN_IRQ = PB0; // irq pin
const uint8_t PIN_SS = PA4;   // spi select pin
 
 
char this_anchor_addr[] = "83:00:22:EA:82:60:3B:9C";
float this_anchor_target_distance = 1; //measured distance to anchor in m
 
uint16_t this_anchor_Adelay = 16610; //starting value
uint16_t Adelay_delta = 100; //initial binary search step size
 
 
void setup()
{
  Serial.begin(115200);
  while (!Serial);

  Wire.begin();
 
  dataLen=sizeof(f);


  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); 
 
 
  Serial.print("Starting Adelay "); Serial.println(this_anchor_Adelay);
  Serial.print("Measured distance "); Serial.println(this_anchor_target_distance);
  
  DW1000.setAntennaDelay(this_anchor_Adelay);
 
  DW1000Ranging.attachNewRange(newRange);
//  DW1000Ranging.attachNewDevice(newDevice);
//  DW1000Ranging.attachInactiveDevice(inactiveDevice);
  //Enable the filter to smooth the distance
  //DW1000Ranging.useRangeFilter(true);
 
  //start the module as anchor, don't assign random short address
  DW1000Ranging.startAsAnchor(this_anchor_addr, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
  DW1000.enableDebounceClock();
  DW1000.enableLedBlinking();
  DW1000.setGPIOMode(MSGP2, LED_MODE);
  DW1000.setGPIOMode(MSGP3, LED_MODE);
 
}
 
void loop()
{

  Wire.requestFrom(slave_address_one,dataLen);
while(Wire.available()==dataLen)
{
I2C_readAnything(f);
Serial.print(f);
}

  DW1000Ranging.loop();
}
 
void newRange()
{
  static float last_delta = 0.0;
  Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
 
  float dist = 0;
  for (int i = 0; i < 100; i++) {
    // get and average 100 measurements
    dist += DW1000Ranging.getDistantDevice()->getRange();
  }
  dist /= 100.0;
  Serial.print(",");
  Serial.print(dist); 
  if (Adelay_delta < 3) {
    Serial.print(", final Adelay ");
    Serial.println(this_anchor_Adelay);
//    Serial.print("Check: stored Adelay = ");
//    Serial.println(DW1000.getAntennaDelay());
    while(1);  //done calibrating
  }
 
  float this_delta = dist - this_anchor_target_distance;  //error in measured distance
 
  if ( this_delta * last_delta < 0.0) Adelay_delta = Adelay_delta / 2; //sign changed, reduce step size
    last_delta = this_delta;
  
  if (this_delta > 0.0 ) this_anchor_Adelay += Adelay_delta; //new trial Adelay
  else this_anchor_Adelay -= Adelay_delta;
  
  Serial.print(", Adelay = ");
  Serial.println (this_anchor_Adelay);
//  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
  DW1000.setAntennaDelay(this_anchor_Adelay);

  pa= dist;
  Serial.print("\t Range: "); Serial.print(pa); Serial.print(" m");
  Wire.beginTransmission(slave_address_one);
  I2C_writeAnything(pa);
  Wire.endTransmission(slave_address_one);
  
  
}
 
//void newDevice(DW1000Device *device)
//{
//  Serial.print("Device added: ");
//  Serial.println(device->getShortAddress(), HEX);
//}
// 
//void inactiveDevice(DW1000Device *device)
//{
//  Serial.print("delete inactive device: ");
//  Serial.println(device->getShortAddress(), HEX);
//}
