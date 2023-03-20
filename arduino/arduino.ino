#include <SPI.h>
#include <Ethernet.h>      // for the module ENC28J60 disable this line
//#include <UIPEthernet.h> // for the module ENC28J60 enable this line. You have to install the library UIPEthernet first
#include <PubSubClient.h>  // The MQTT library

#include<I2C_Anything.h>
#include<Wire.h>


const char* mqtt_server = "broker.hivemq.com"; // replace with your broker url
const int mqtt_port =1883; // unsecure port


// Initialize the Ethernet server library with the port you want to use.
EthernetServer server(80);
String readString;
// you don't need the username and password for public connection
EthernetClient ethClient;
PubSubClient client(ethClient);
byte mac[] = {0xB0,0xCD,0xAE,0x0F,0xDE,0x8 };
#define LED_PIN 8
unsigned long lastMsg = 0;

int sensor1 = 0;   // temperature value
float sensor2 = 1;  // id


const char* sensor1_topic= "Range2";  // replace the prefix "username"
const char*  sensor2_topic="ID3";  



const byte MY_address=8;
float y;



//================================================ setup
//================================================
void setup() {
    Serial.begin(9600);
  while (!Serial) delay(1);
  Ethernet.begin(mac);
  pinMode(LED_PIN, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  //--- init mqtt client
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
 // client.setBufferSize(255);  // default 128
  randomSeed(micros());
  String clientId = "EthernetClient-";   // Create a random client ID
  clientId += String(random(0xffff), HEX);
  if (client.connect(clientId.c_str())) {
    Serial.println("connected");

   
    //client.subscribe(command2_topic);   
  } 
  else {
    Serial.print("\nfailed, rc=");
    Serial.println(client.state());
    }

     Wire.begin(MY_address);
  Serial.begin(9600);
  Wire.onReceive(event2);
}


//================================================ loop
//================================================
void loop() {
  client.loop();

  //---- example: how to publish sensor values every 5 sec
  unsigned long now = millis();
  if (now - lastMsg > 500) {
    lastMsg = now;
    
    publishMessage(sensor1_topic,String(y),true);  // publish the sensor1 value to the broker  
    publishMessage(sensor2_topic,String(sensor2),true);
    
  }
}

//=======================================  
// This void is called every time we have a message from the broker

void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage+=(char)payload[i];
  
  Serial.println("Message arrived ["+String(topic)+"]"+incommingMessage);
  
  //--- check the incomming message

 //  check for other commands
 //  else  if( strcmp(topic,command2_topic) == 0){
 //    if (incommingMessage.equals("1")) {  } // do something else
 // }
}



//======================================= publising as string
void publishMessage(const char* topic, String payload , boolean retained){
  if (client.publish(topic, payload.c_str(), true))
      Serial.println("Message publised ["+String(topic)+"]: "+payload);
}

void event2()
{
  I2C_readAnything(y);
  Serial.print("    " );
  Serial.println(y);
  delay(1000);
}
