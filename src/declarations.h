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
#include <stdio.h>       /* puts */
#include <time.h>        /* time_t, struct tm, time, localtime, strftime */
#include <iostream>
#include <string.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiManager wifiManager;

//The mqtt_server address is the ip address of the local computer that is running
//your mqtt broker. This address should be set as static!
const char *mqtt_server = "192.168.1.11";

//Unique ID used to identify inividual ESP32 chips
char ClientID[23]; //ESP unique ID number (Automatically created during setup- always the same)

//define your default values here, if there are different values in config.json,
//they are overwritten.
char UnitID[8] = "0";      //Unit number is assigned '0' to test for config completion
char HourMeter[6] = "0";   //Keeps track of hours of operation
char PMI_Months[3] = "12"; //Number of Months to wait to trigger a PMI
char PMI_Hrs[4] = "250";   //Number of hours to wait until triggering a PMI
char Last_PMI[11] = "N/A"; //Date last PMI completed
char Now[11] = "N/A";      //Current Date and time

const int PMI_Extend = 10; //Number of hours to temporarily add to overdue PMI

unsigned long startTime = 0;
unsigned long stopTime = 0;
unsigned long runTime = 0;

/* Two "independant" timed events */
const long E1 = 10000; //Event 1 in ms
const long E2 = 5000;  //Event 2 in ms

/* Mark when last time the Event was set */
unsigned long Last_E1 = 0; //Starts out as 0
unsigned long Last_E2 = 0; //Starts out as 0

#define AP_REQUEST 25
#define RUN_SENSOR 13

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR unsigned long totTime = 0;

boolean runSensorState = false; //Unit not running

//All functions must be delcared here
void openAP();
void createChipID();
void mqttConnect();
void readConfig();
void saveConfig();
void getAscTime(char *ptr_time);
void updateConfig(byte *payload, unsigned int length);
void dataInCallback(char *topic, byte *payload, unsigned int length);

void goToSleep(); //Sub declaration required by ccp
void addToHM();
