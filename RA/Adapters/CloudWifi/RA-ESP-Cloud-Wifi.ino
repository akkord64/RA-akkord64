////////////////////////
// Debug Flag
//#define VERBOSE
//////////////////////
#ifdef VERBOSE
#define VERBOSE_PRINT(str) Serial.print(str)
#define VERBOSE_PRINTLN(str) Serial.println(str)
#else
#define VERBOSE_PRINT(str)
#define VERBOSE_PRINTLN(str)
#endif
////////////////////////
//#define DEBUG
///////////////////////
#ifdef DEBUG
#define DEBUGPRINT(str) Serial.println(str)
#else
#define DEBUGPRINT(str)
#endif
////////////////////////

//#include <WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <SPI.h>

SoftwareSerial SerialS (14, 12, false, 256);

 
#include <ESP8266WiFi.h>
const char* ssid = "<CHANGE TO SSID>";
const char* password =  "<CHANGE TO PASSWORD>";
const char* mqttServer = "cloud.reefangel.com";
const char* PortalServer = "forum.reefangel.com";
const int mqttPort = 1883;
const char* mqttUser = "<CHANGE TO USER>";
const char* mqttPassword = "<CHANGE TO PASSWORD>";
const char* mqttClientid = "<CHANGE TO CLIENTID>";
const char* intopic = "<CHANGE TO INTOPIC>#";
const char* outtopic = "<CHANGE TO OUTTOPIC>";
const int ledPin =  2;
unsigned long client_timeout = millis();
unsigned long wifi_connection = millis();
unsigned long serial_timeout = millis();
unsigned long led_blink = millis();
unsigned long MQTTReconnectmillis = millis();
boolean LED_status = false;
boolean PortalConnection = false;
boolean power_status = true;
boolean data_ready = false;
int status = WL_IDLE_STATUS;
WiFiServer server(2000);
WiFiClient portalclient;
WiFiClient client; 
WiFiClient mqttClient;
PubSubClient CloudClient(mqttClient);
String currentLine = "";                // make a String to hold incoming data from the client


void mqttCallback(char* topic, byte* payload, unsigned int length){
  VERBOSE_PRINT("cloud:");
  SerialS.print("cloud:");
  for (int a=0;a<length;a++)
  {
    VERBOSE_PRINT(payload[a]);
    SerialS.write(payload[a]);
    
  }
  VERBOSE_PRINT(" ");
  SerialS.print(" ");
}

void SendData(){
  serial_timeout = millis();
  client.print(currentLine);
  VERBOSE_PRINT(currentLine); // Print what is being sent to Portal
  currentLine = "";
}
 
void setup() {
 
  Serial.begin(57600);
  SerialS.begin(57600);
  WiFi.begin(ssid, password);
  pinMode(ledPin, OUTPUT);      // set the LED pin mode
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
   Serial.println("Connected to the WiFi network");
   CloudClient.setServer(mqttServer, mqttPort);
   CloudClient.setCallback(mqttCallback);
  while (!CloudClient.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (CloudClient.connect(mqttClientid, mqttUser, mqttPassword)) {
      CloudClient.subscribe(intopic);
      Serial.println("connected");
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(CloudClient.state());
      delay(2000);
 
    }
  }
 
//client.publish("esp/test", "Hello from ESP32");
 
}
 
void loop() {
  //DEBUGPRINT("Restarting Loop");
  //delay (1000);
  status = WiFi.status();
  CloudClient.loop();
     if (!CloudClient.connected())
        {
          ESP.restart();
        }
  client = server.available();   // listen for incoming clients 
//------------------------BEGIN HTTP Client Handle-------------------------------------------------------------------  
  if (client) {                             // if you get a client,
    DEBUGPRINT("Waiting on HTTP Client");
    client_timeout = millis();
    while (client.connected()) {            // loop while the client's connected
      DEBUGPRINT("HTTP Client Connected");
      if (millis() - client_timeout > 2000) client.stop();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        SerialS.write(c);                    // print it out the serial port to RA
        VERBOSE_PRINT("Portal Data: "); // Print what is being sent to RA
        VERBOSE_PRINTLN(c); // Print what is being sent to RA
      }
      if (SerialS.available())
      {
        char c = SerialS.read();
        currentLine += c;
        serial_timeout = millis();
        if (currentLine.length() == 255) {
          SendData();
          }
      }
      if (millis() - serial_timeout > 400)
      {
        if (currentLine.length() > 0) {
          SendData();
          client.println();
          VERBOSE_PRINTLN("Serial Port has not sent a resonse in over 400msec"); // Print this in case we reach this serial timeout
        }
      }
      if (millis() - led_blink > 50)
      {
        led_blink = millis();
        LED_status = !LED_status;
        digitalWrite(ledPin, LED_status);
      }
    }
    client.stop();
    DEBUGPRINT("HTTP Client Stopped");
    digitalWrite(ledPin, false);
  }

//------------------------END HTTP Client Handle-------------------------------------------------------------------  
//------------------------Begin Incoming Serial Data Handle---------------------------------------------------------------------- 
  else
  {
    while (SerialS.available())
    {
      DEBUGPRINT("Starting Portal Handle Loop");
      char c = SerialS.read();
      currentLine += c;
      if(c==10) data_ready=true;
      serial_timeout = millis();
      digitalWrite(ledPin, true);
      if (millis() - serial_timeout > 200 || data_ready)
      {
        if (currentLine.length() > 0) 
        {
          data_ready=false;
          serial_timeout = millis();
          VERBOSE_PRINTLN(currentLine);
if (currentLine.startsWith("GET"))
          {
            PortalConnection = true;
            Serial.println(F("Connecting to Portal Server"));
            if (portalclient.connect(PortalServer, 80)) {
              Serial.println(F("Connected"));
              // Make a HTTP request:
              portalclient.print(currentLine);
              portalclient.println(F(" HTTP/1.1"));
              portalclient.println(F("Host: forum.reefangel.com"));
              portalclient.println(F("Connection: close"));
              portalclient.println();
            }
            else
            {
              Serial.println(F("Failed to connect"));
            }
          }
          if (currentLine.startsWith("CLOUD:"))
          {
            if (currentLine.length() < 32)
            {
              if (CloudClient.connected())
              {
                VERBOSE_PRINTLN("MQTT Update from RA"); // Print this in case we see this message from RA
                currentLine.replace("CLOUD:", "");
                char pub_buffer[sizeof(mqttUser) + 5];
                sprintf(pub_buffer, "%s/out", mqttUser);
                char pub_msg[currentLine.length() + 1];
                currentLine.toCharArray(pub_msg, currentLine.length() + 1);
                CloudClient.publish(outtopic, pub_msg);
                VERBOSE_PRINT("MQTT Topic :"); // Print the MQTT Topic
                VERBOSE_PRINTLN(outtopic); // Print the MQTT Topic
                VERBOSE_PRINT("MQTT Payload :"); // Print the MQTT Payload
                VERBOSE_PRINTLN(pub_msg); // Print the MQTT Payload
              }
            }
            else
            {
              Serial.print(F("Error on incoming data: "));
              Serial.println(currentLine.length());
            }
          }
          currentLine = "";
          digitalWrite(ledPin, false);
        }
      }
    }
        digitalWrite(ledPin, false);  
    while (portalclient.available()) {
      char c = portalclient.read();
    }
    if (PortalConnection && !portalclient.connected()) {
      PortalConnection = false;
      VERBOSE_PRINTLN(F("Disconnecting from Portal Server")); // Nothing else from portal; disconnect.
      digitalWrite(ledPin, false);
      portalclient.stop();
    }
   }
}
