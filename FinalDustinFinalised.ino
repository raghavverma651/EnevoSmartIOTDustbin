#include <WifiLocation.h>
#include <FirebaseArduino.h>
#include <Arduino.h>
#ifdef ARDUINO_ARCH_SAMD
#include <WiFi101.h>
#elif defined ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#elif defined ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#error Wrong platform
#endif 
const char* googleApiKey = "AIzaSyCB4-OBThLgfivj4bbg7qH7rRvzIGdn9Ao";
const char* ssid = "pinchu";
const char* passwd = "pinchu0499";
#define TRIG_PIN 2 //D4 (GPI02 Pin)
#define ECHO_PIN 0 //D3 (GPI00 Pin)
#define FIREBASE_HOST "smartbin-70cb3.firebaseio.com"
#define FIREBASE_AUTH "JfKpn9F81YdCnUtbqOyK0USTZic109rf6XyBltMR"
long duration;
int distance;

WifiLocation location(googleApiKey); //This is used to parse the geolocation by tracking down the IP Address via Google's A PI

void setup() {
    Serial.begin(115200); //Upload Speed
    pinMode(TRIG_PIN, OUTPUT); //Setup for trigger pin
    pinMode(ECHO_PIN, INPUT); //Setup for echo pin
#ifdef ARDUINO_ARCH_ESP32 //Setup for ESP32 incase the latter doesnt work
    WiFi.mode(WIFI_MODE_STA); //WiFi station setup
#endif
#ifdef ARDUINO_ARCH_ESP8266 //setup for our original wifi module
    WiFi.mode(WIFI_STA);
#endif
    WiFi.begin(ssid, passwd); //Sets up the connection to internet
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print("Attempting to connect to WPA SSID: ");
        Serial.println(ssid);
        Serial.print("Status = ");
        Serial.println(WiFi.status()); //Status 6 implies that the wifi microcontroller is currently connecting to the WiFi
        delay(500);
    }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  location_t loc = location.getGeoFromWiFi(); //Object for the geolocation of the Wifi's Local IP
  Serial.println("Location request data");
  Serial.println("Latitude: " + String(loc.lat, 7));
  Serial.println("Longitude: " + String(loc.lon, 7));
  Serial.println("Accuracy: " + String(loc.accuracy));
  Serial.println();
  Firebase.setFloat("Latitude",loc.lat); //Updating the values of latitude to the realtime database of Firebase Cloud Platform
  Firebase.setFloat("Longiitude",loc.lon); //Updating the values of longitude to the realtime database of Firebase Cloud Platform
}

void loop() 
{
    digitalWrite(TRIG_PIN, LOW); //Trigger pin at low voltage
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH); //Trigger pin at high voltage
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW); //Trigger pin at low voltage. At this point a pulse is created 
    long duration = pulseIn(ECHO_PIN, HIGH); //The duration of this pulse in recorded into a variable
    float distance = duration*0.017; //This duration is converted into distance using the speed of sound formula
    Serial.print("Distance (cm): ");
    Serial.println(distance,3);
    Firebase.setFloat("Distance", distance); //Thus the distance value is updated on Firebase
    
    if (Firebase.failed()) 
    {
      Serial.print("setting /number failed:"); //In case the updation fails, this message appears on the Serial Monitor
      Serial.println(Firebase.error());  
      return;
    }
    delay(10);
}
