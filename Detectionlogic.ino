#define TS_ENABLE_SSL // For HTTPS SSL connection

#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

const int trigPin = 2; //pinvalue
const int echoPin = 18;
const int motionPin = 27;
const int micPin = 26;
int lastState = HIGH; 
int currentState; 

long duration;
float distanceCm;
bool motionDetected;
bool soundDetected;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motionPin, INPUT);
  pinMode(micPin, INPUT);
  pinMode(32, OUTPUT); // Red LED
  pinMode(33, OUTPUT); // Green LED
  pinMode(25, OUTPUT); // Yellow LED
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;

  motionDetected = digitalRead(motionPin);
  int ledstatus;

  if (motionDetected) {
    //Serial.println("Motion Detected");
  }
    digitalWrite(32, HIGH); // Red -- Green
    digitalWrite(33, HIGH);  // Green -- Red
    digitalWrite(25, HIGH);  // Yellow off
  if (distanceCm < 30 && motionDetected) {
    Serial.println("Possible activity");
    
    digitalWrite(33, LOW);  // Green -- Red
    
    ledstatus = 0;
  } else if (distanceCm >= 30 && distanceCm <= 60 && motionDetected) {
    Serial.println("Obstacle in moderate range");
    
    digitalWrite(25, LOW); // Yellow on
    ledstatus = 2;
  } else {
    Serial.println("No activity");
    digitalWrite(32, LOW);  // Red off
    
    ledstatus = 1;
  }

  if(WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  // Write to ThingSpeak
  int x = ThingSpeak.writeField(myChannelNumber, 1, ledstatus , myWriteAPIKey);
  if(x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(20000); // Wait 20 seconds to update the channel again
}