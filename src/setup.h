#pragma once
#include "functions.h"

void setup() {

  Serial.begin(115200);
  pinMode(AP_REQUEST, INPUT_PULLUP);
  pinMode(RUN_SENSOR, INPUT_PULLUP);
  delay(1000);
  WiFi.mode(WIFI_STA);
  Serial.println("Attempting Auto WiFi Connect!)");
  wifiManager.autoConnect("AutoConnectAP");

  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(dataInCallback);

  createChipID();

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0);

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  Serial.println("Total accumulated time : " + String(totTime) + " seconds");

  Serial.println();

  runTime = 0;
  startTime = millis();

  readConfig();
  int cmp = (strcmp(UnitID, "0"));

  if (cmp == 0) {
    Serial.println(F("UnitID does = 0"));
    strcpy(UnitID, "N/A");
    saveConfig();
  }
}