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
char ClientID[23]; //ESP unique ID number (Automatically created during setup- always the same)

//define your default values here, if there are different values in config.json,
//they are overwritten.
char UnitID[8] = "0";      //Unit number is assigned '0' to test for config completion
char HourMeter[6] = "0";   //Keeps track of hours of operation
char PMI_Months[3] = "12"; //Number of Months to wait to trigger a PMI
char PMI_Hrs[4] = "250";   //Number of hours to wait until triggering a PMI
char Date[11] = "N/A";     //Date last PMI completed

const int PMI_Extend = 10; //Number of hours to temporarily add to overdue PMI

unsigned long startTime = 0;
unsigned long stopTime = 0;
unsigned long runTime = 0;

unsigned long lastNow = 0; //used by mqttConnect (non-blocking)

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
