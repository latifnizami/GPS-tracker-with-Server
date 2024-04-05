#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h> 
#include <ThingSpeak.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Bus1"
#define WIFI_PASSWORD "0987654321"
// unsigned long myChannelNumber = "ENTER CHANNEL NUMBER";
// const char * myWriteAPIKey = "ENTER THINKSPEAK API KEY";
// Insert Firebase project API Key
#define API_KEY "AIzaSyA9M0MYJXQXf_Ynu4rdVvzQF8m8Lq-8tPQ"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://bus-tracking-19e2d-default-rtdb.firebaseio.com/" 
float latitude;
float longitude ;
  int speed;
  
//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
#define FIREBASE_HOST "bus-tracking-19e2d-default-rtdb.firebaseio.com"  
#define FIREBASE_AUTH "LeiXLBot3qFVXf0AlbFLEJD1muPPeTpJ0ZMSTvhl"  
//GPS Module RX pin to NodeMCU D1
//GPS Module TX pin to NodeMCU D2
const int RXPin = 5, TXPin = 4;
SoftwareSerial neo6m(RXPin, TXPin);
TinyGPSPlus gps;

void setup()
{
  Serial.begin(115200);
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
  neo6m.begin(9600);
 

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  //   ThingSpeak.begin(client);
}

 
void loop()
{
   firebasedata();
  //smartdelay_gps(500);
   while (neo6m.available() > 0) {
  //   // Read the GPS data
   if (gps.encode(neo6m.read())) {
  //     // Check if the GPS data is valid
  if(gps.location.isValid()) 
  {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    speed = gps.speed.kmph();
  Serial.println("latitude:=");
  Serial.println(latitude);
  Serial.println("longitude:=");
  Serial.println(longitude);
  Serial.println("speed:=");
  Serial.println(speed);
 firebasedata();
}
}
 }
}
  void firebasedata()
  {

  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an float number on the database path test/int
    if (Firebase.RTDB.setFloat(&fbdo, "Bus_Tracking/Latitude",0.01+latitude )){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
      if (Firebase.RTDB.setFloat(&fbdo, "Bus_Tracking/Longitude",0.01+longitude)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
  else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  // if (Firebase.RTDB.setInt(&fbdo, "Bus_tracking/Speed",speed)){
  //     Serial.println("PASSED");
  //     Serial.println("PATH: " + fbdo.dataPath());
  //     Serial.println("TYPE: " + fbdo.dataType());
  //   }
  // else {
  //     Serial.println("FAILED");
  //     Serial.println("REASON: " + fbdo.errorReason());
  //   }
 
}
  }
  static void smartdelay_gps(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (neo6m.available())
      gps.encode(neo6m.read());
  } while (millis() - start < ms);
}