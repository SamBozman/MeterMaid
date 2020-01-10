#pragma once
#include "functions.h"

void setup() //Runs once when first starting program (reset)
{

  Serial.begin(115200);
  pinMode(AP_REQUEST, INPUT_PULLUP);
  pinMode(RUN_SENSOR, INPUT_PULLUP);
  delay(1000); //Take some time to open up the Serial Monitor
  WiFi.mode(WIFI_STA);
  Serial.println("Attempting Auto WiFi Connect!)");
  wifiManager.autoConnect("AutoConnectAP");
  //Program will be blocked here until WiFi has been set up.

  mqttClient.setServer(mqtt_server, 1883); //Ip address, port#
  mqttClient.setCallback(dataInCallback);

  createChipID(); //Creates a unique chip ID used for communication

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0); //WAKE UP CHIP ON LOW

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  Serial.println("Total accumulated time : " + String(totTime) + " seconds");

  Serial.println();
  delay(3000); //delay long enough to read the print lines above

  runTime = 0;          // reset runTime
  startTime = millis(); //Mark the beginning of our accumulated time for hourmeter

  // FIXME:   Test that this following code works properly

  readConfig();                    //If there is no config file then UnitID will stay as '0'
  int cmp = (strcmp(UnitID, "0")); //If UnitID still equals '0' then ...
  if (cmp == 0)
  {
    Serial.println(F("UnitID does = 0"));
    strcpy(UnitID, "N/A"); //Change UnitID to N/A until config file is located
    saveConfig();          //Save temporary default configuration with UnitID = 'N/A'
  }
}