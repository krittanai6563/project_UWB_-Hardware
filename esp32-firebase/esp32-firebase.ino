//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "M"
#define WIFI_PASSWORD "11111111"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyC2l21YBFXUdqz61ukwaRXcEynt7t32C30"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://projectuwb-27b59-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "test@gmail.com"
#define USER_PASSWORD "123456"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//create an RF24 object
RF24 radio(4, 5);  // CE, CSN

float text;
float text1;
float text2;


//address through which two modules communicate.
const byte address[6] = "00001";


unsigned long sendDataPrevMillis = 0;
unsigned long count = 0;
long randNumber;

void setup()
{
  while (!Serial);
    Serial.begin(9600);
  
  radio.begin();
  
  //set the address
  radio.openReadingPipe(0, address);
  radio.openReadingPipe(1, address);
  radio.openReadingPipe(2, address);
  
  //Set module as receiver
  radio.startListening();


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

#if defined(ESP8266)
 
  fbdo.setBSSLBufferSize(2048 /* Rx buffer size in bytes from 512 - 16384 */, 2048 /* Tx buffer size in bytes from 512 - 16384 */);
#endif

  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;
  
  
}

void loop()
{
  //Read the data if available in buffer
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();
    FirebaseJson json;

//      randNumber = random(300);
//   Serial.println(randNumber);

    if (count == 0)
    {
      json.set("value/round/" + String(count), F("0"));
      json.set(F("value/ts/.sv"), F("timestamp"));
      Serial.printf("Set json... %s\n", Firebase.RTDB.set(&fbdo, F("/test/json"), &json) ? "ok" : fbdo.errorReason().c_str());
      
    }
    
    else
    {

        if (radio.available())
     {
    
    radio.read(&text, sizeof(text));
    Serial.print("ค่าจากบอร์ดที่ 1 ");
    Serial.println(text);

    radio.read(&text1, sizeof(text1));
    Serial.print("ค่าจากบอร์ดที่ 2 ");
    Serial.println(text1);

    radio.read(&text2, sizeof(text2));
    Serial.print("ค่าจากบอร์ดที่ 3 ");
    Serial.println(text2);
      }
      
      json.add(String(count), float(text));
      Serial.printf("Update node... %s\n", Firebase.RTDB.updateNode(&fbdo, F("/esp32/json/Anchor1"), &json) ? "ok" : fbdo.errorReason().c_str());

      json.add(String(count), float(text1));
      Serial.printf("Update node... %s\n", Firebase.RTDB.updateNode(&fbdo, F("/esp32/json/Anchor2"), &json) ? "ok" : fbdo.errorReason().c_str());

      json.add(String(count), float(text2));
      Serial.printf("Update node... %s\n", Firebase.RTDB.updateNode(&fbdo, F("/esp32/json/Anchor3"), &json) ? "ok" : fbdo.errorReason().c_str());
    }
    Serial.println();
    count++;
   
  }



}
