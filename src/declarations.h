#pragma once

//#define _GLIBCXX_USE_C99 1
#include "SPIFFS.h"
#include <Arduino.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJsonn
#include <DNSServer.h>
#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <PubSubClient.h>
#include <WebServer.h>
#include <WiFi.h> //https://github.com/esp8266/Arduino
//#include <WiFiManager.h>
////https://github.com/tzapu/WiFiManager/tree/development
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>

#include <ESP_WiFiManager.h> //https://github.com/khoih-prog/ESP_WiFiManager
#include <WiFiClient.h> // TODO change WiFiClient to WiFiClientSecure (Must also use port 443)
// Link to more information: https://www.youtube.com/watch?v=Wm1xKj4bKsY
#include <esp_wifi.h>

using namespace std;
WiFiClient mqttWiFiClient;
PubSubClient mqttClient;
// WiFiManager wifiManager;
const char *mqtt_server = "192.168.1.127";
// TODO give mqtt server a name??

String Router_SSID;
String Router_Pass; // TODO Setup WiFi password hash (1)
String MacAddress = WiFi.macAddress();
char ClientID[21]; //Used to hold ClientID 
char ClientID_t[21]; //used to get date/time from server
const char *apPwd = "mmesp32";


char UnitID[8] = "0";
char HourMeter[6] = "0";
char PMI_Months[3] = "12";
char PMI_Hrs[4] = "250";
char Last_PMI[11] = "N/A";
char Last_MQTT[11] = "N/A";

unsigned long int CurrentTime = 0;

unsigned long startTime = 0;
unsigned long stopTime = 0;
unsigned long runTime = 0;

const long MQTT_Interval =
    60000; // 1 min for testing. Should be at least 15 min for production
unsigned long startMQTT_Millis;
unsigned long currentMQTT_Millis;

#define AP_REQUEST 0 // Built in boot switch
#define RUN_SENSOR 13
#define AP_LED 2

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR unsigned long totTime = 0;

boolean runSensorState = false;

// All functions must be delcared here
void openAP();
void createChipID();
bool mqttConnect();
void readConfig();
void saveConfig();
void updateConfig(byte *payload, unsigned int length);
void dataInCallback(char *topic, byte *payload, unsigned int length);
char const *getSubscription(const char *in);
void goToSleep();
void addToHM();
//void strToUnix(char *ptr);
char *strptime(const char *__restrict, const char *__restrict,
               struct tm *__restrict);
//void heartBeatPrint(void);
void check_status();
void configWiFi();