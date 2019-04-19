#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>

/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

static const int RXPin = 4, TXPin = 5;
static const uint32_t GPSBaud = 9600;

// repace your wifi username and password
const char* ssid     = "Redmi";
const char* password = "12345678";


char auth[] = "1b371917a55947d0b853758e27cd0002";
unsigned long myChannelNumber = 736011;
const char * myWriteAPIKey = "3U8KPQGVODVTS309";

const byte interruptPin1 = D5;
const byte interruptPin2 = D6;
const byte interruptPin3 = D7;

volatile unsigned long previousmillis=0;
volatile unsigned long currentmillis=0;
volatile unsigned long prev_millis=0;
volatile unsigned long cur_millis=0;
volatile int count =0;
volatile int entered =0;
volatile int exited =0;
volatile long interval=1000;
volatile boolean flag=LOW;
unsigned int move_index = 1; 
// The TinyGPS++ object
TinyGPSPlus gps;
WiFiClient  client;
WidgetMap myMap(V0);
BlynkTimer timer;
// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);
  
  Blynk.begin(auth,ssid,password);
  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
   Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Netmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
 ThingSpeak.begin(client);
  pinMode(interruptPin1, INPUT);
  pinMode(interruptPin2, INPUT);
  pinMode(interruptPin3, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin1), enter1, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), enter2, RISING);
  attachInterrupt(digitalPinToInterrupt(interruptPin3), exit1, RISING);
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.

  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();
      Blynk.run();
      timer.run(); 
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

void displayInfo()
{
 // Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {

    double latitude = (gps.location.lat());
    double longitude = (gps.location.lng());
    
    String latbuf;
    latbuf += (String(latitude, 6));
   Serial.println(latbuf);

    String lonbuf;
    lonbuf += (String(longitude, 6));
    Serial.println(lonbuf);
    Blynk.virtualWrite(V1,latbuf+lonbuf);
    myMap.location(move_index, latbuf, lonbuf, "GPS_Location");
    ThingSpeak.setField(1, latbuf);
    ThingSpeak.setField(2, lonbuf);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);  
    delay(2000);
    
  }
  else
  {
    Serial.print(F("INVALID"));
  }
  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

void enter1(){
  flag=HIGH;
  previousmillis = millis();
  //Serial.println("Entered");
}
void enter2(){
  if (flag==HIGH||(currentmillis-previousmillis<interval)){
    entered++;
    count++;
   Serial.println(count);
   Blynk.virtualWrite(V2, count);
   ThingSpeak.setField(3, entered);
   ThingSpeak.setField(5, count);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  flag=LOW;
  }
}
void exit1(){
  exited++;
  count--;
  Serial.println(count);
  Blynk.virtualWrite(V2, count);
   ThingSpeak.setField(4, exited);
   ThingSpeak.setField(5, count);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}
