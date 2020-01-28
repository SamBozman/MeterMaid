#pragma once
// INCLUDES
//#define _GLIBCXX_USE_C99 1
#include "SPIFFS.h"
#include <Arduino.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJsonn
#include <DNSServer.h>
#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <PubSubClient.h>
#include <WebServer.h>
#include <WiFi.h>        //https://github.com/esp8266/Arduino
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager/tree/development
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>
using namespace std;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiManager wifiManager;
const char *mqtt_server = "192.168.1.11";

char ClientID[11];
char ClientID_t[13];

char UnitID[8] = "0";
char HourMeter[6] = "0";
char PMI_Months[3] = "12";
char PMI_Hrs[4] = "250";
char Last_PMI[11] = "N/A";
unsigned long int CurrentTime = 0;

unsigned long startTime = 0;
unsigned long stopTime = 0;
unsigned long runTime = 0;

const long EventInterval1 = 10000;
const long EventInterval2 = 5000;

unsigned long Last_Event1 = 0;
unsigned long Last_Event2 = 0;
unsigned long now = 0;

#define AP_REQUEST 25
#define RUN_SENSOR 13

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR unsigned long totTime = 0;

boolean runSensorState = false;

// All functions must be delcared here
void openAP();
void createChipID();
void mqttConnect();
void readConfig();
void saveConfig();
void updateConfig(byte *payload, unsigned int length);
void dataInCallback(char *topic, byte *payload, unsigned int length);
char const *getSubscription(const char *in);
void goToSleep();
void addToHM();
void strToUnix(char *ptr);