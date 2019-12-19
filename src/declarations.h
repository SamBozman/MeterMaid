#pragma once
//INCLUDES
#include <FS.h> //this needs to be first, or it all crashes and burns...
#include "SPIFFS.h"
#include <Arduino.h>
#include <WiFi.h> //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager/tree/development
#include <PubSubClient.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJsonn

WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiManager wifiManager;


//Unique ID used to identify inividual ESP32 chips
char ClientID[23];//ESP unique ID number (Automatically created during setup- always the same)


//define your default values here, if there are different values in config.json,
//they are overwritten.
//char UnitID[12] = "N/A"; //Assigned by the database
char UnitID[16];
char HourMeter[8] = "0";//Keeps track of hours of operation
char PMI_Months[3] = "12";//Number of Months to wait to trigger a PMI
char PMI_Hrs[4] = "250";//Number of hours to wait until triggering a PMI
char PMI_Extend[4] = "10";//Number of hours to temporarily add to overdue PMI
// to extend time before issuing a new PMI_Due message
int HM; //Used to convert char HourMeter into an int
//char PMI_Interval[6] = "250";
//char PMI_Extend[4] = "10";
//char WiFi_Retry[4];



long lastMsg = 0;
char msg[50]; //String containing out-going Publish message
long value = 0;

// select which pin will trigger the configuration portal when set to LOW
// ESP-01 users please note: the only pins available (0 and 2), are shared
// with the bootloader, so always set them HIGH at power-up
unsigned long startTime = 0;
unsigned long stopTime = 0;
unsigned long runTime = 0;

#define AP_REQUEST 25
#define RUN_SENSOR 13

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR unsigned long totTime = 0;

boolean runSensorState = false; //Unit not running

void mqttConnect();
void dataInCallback(char *topic, byte *payload, unsigned int length);
void saveConfig();
void goToSleep(); //Sub declaration required by ccp
void openAP();
void addToHM();
void readConfig();
void createChipID();
