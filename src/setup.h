#pragma once
#include "functions.h"

void setup() //Runs once when first starting program (reset)
{

  Serial.begin(115200);
  pinMode(AP_REQUEST, INPUT_PULLUP);
  pinMode(RUN_SENSOR, INPUT_PULLUP);
  delay(1000); //Take some time to open up the Serial Monitor

  //The mqtt_server address is the ip address of the local computer that is running
  //your mqtt broker. This address should be set as static!
  Serial.println("Attempting Auto WiFi Connect!)");
  wifiManager.autoConnect("AutoConnectAP");
  //Program will be blocked here until WiFi has been set up.
  Serial.println("SUCCESS - Auto Connected during setup!)");

  //FIXME: This address has been set as static
  const char *mqtt_server = "192.168.1.11";
  mqttClient.setServer(mqtt_server, 1883); //Ip address and port #
  mqttClient.setCallback(dataInCallback);
  createChipID(); //Creates a unique chip ID used for communication

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0); //WAKE UP CHIP ON LOW

  //Increment boot number and print it every reboot
  Serial.println("Boot number: " + String(bootCount));
  ++bootCount;

  lastNow = 0; //used by mqttConnect (non-blocking)
  runTime = 0; // reset runTime
  startTime = millis();
  Serial.println("Unit running, Starting Timer. ");

  // FIXME:   Test that this following code works properly

  readConfig();                    //If there is no config file then UnitID will stay as '0'
  int cmp = (strcmp(UnitID, "0")); //If UnitID still equals '0' then ...
  if (cmp == 0)
  {
    Serial.println(F("UnitID does = 0"));
    strcpy(UnitID, "N/A"); //Change UnitID to N/A until config file is located
    saveConfig();          //Save temporary default configuration with UnitID = 'N/A'
  }
  else
  {
    Serial.print(F("UnitID = "));
    Serial.println(F(UnitID));
  }
}