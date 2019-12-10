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

  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("SUCCESS - Auto Connected during setup!)");
  const char *mqtt_server = "192.168.1.11"; //This address has been set as static

  mqttClient.setServer(mqtt_server, 1883); //Ip address and port #
  mqttClient.setCallback(dataInCallback);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0); //WAKE UP CHIP ON LOW
  createChipID();                               //Creates a unique chip ID used for communication

  //Increment boot number and print it every reboot
  Serial.println("Boot number: " + String(bootCount));
  ++bootCount;

  runTime = 0; // reset runTime
  startTime = millis();
  Serial.println("Unit running, Starting Timer. ");

  //read configuration from FS json
  Serial.println("Attempt to read json/config file...");
  readConfig();
}