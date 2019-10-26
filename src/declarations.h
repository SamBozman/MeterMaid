#pragma once
#include "includes.h"

//define your default values here, if there are different values in config.json,
//they are overwritten by these values.

char HourMeter[8];
int HM;

char PMI_Interval[6];
char PMI_Extend[4];
char WiFi_Retry[4];
const char* mqtt_server = "192.168.1.91";

WiFiClient espClient;
PubSubClient mqttClient(espClient);
uint64_t chipid;
char ClientID[23];
uint16_t chip;
long lastMsg = 0;
char msg[50];
long value = 0;
String jsonPayload; //hold serialization of json object for MQTT::Publish

char test[6] = "1234";
int x;

WiFiManager wifiManager;


//flag for saving data
bool shouldSaveConfig = false;

// select which pin will trigger the configuration portal when set to LOW
// ESP-01 users please note: the only pins available (0 and 2), are shared
// with the bootloader, so always set them HIGH at power-up
unsigned long startTime = 4294967295;
unsigned long stopTime = 0;
unsigned long runTime = 0;

#define AP_REQUEST 25
#define RUN_SENSOR 13

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR unsigned long totTime = 0;
boolean runSensorState = false; //Unit not running

void mqttConnect();
void dataInCallback(char* topic, byte* payload, unsigned int length);
void saveConfig();
void goToSleep(); //Sub declaration required by ccp
void saveConfigCallback();
void openAP();
void addToHM();