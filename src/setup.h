#pragma once
#include "functions.h"

void setup() //Runs once when first starting program (reset)
{
  
  Serial.begin(115200);
  pinMode(AP_REQUEST, INPUT_PULLUP);
  pinMode(RUN_SENSOR, INPUT_PULLUP);
  delay(1000); //Take some time to open up the Serial Monitor



  //Increment boot number and print it every reboot
  Serial.println("Boot number: " + String(bootCount));
  ++bootCount;

  runTime = 0; // reset runTime
  startTime = millis();
  Serial.println("Unit running, Starting Timer. ");
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0); //0 = Low (GND)
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("SUCCESS - Auto Connected during setup!)");
  
 
 
  
  //read configuration from FS json
  Serial.println("Attempt to read json/config file...");
  readConfig();

  
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(dataInCallback);

  chipid = ESP.getEfuseMac(); //The chip ID is essentially its MAC address(length: 6 bytes).
  chip = (uint16_t)(chipid >> 32);
  snprintf(ClientID, 23, "ESP-%04X%08X", chip, (uint32_t)chipid);
  Serial.print("The chip ID is ");
  Serial.println(ClientID);
}