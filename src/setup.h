#pragma once
#include "functions.h"

void setup() {

  Serial.begin(115200);
  pinMode(AP_LED, OUTPUT);
  pinMode(AP_REQUEST, INPUT_PULLUP);
  pinMode(RUN_SENSOR, INPUT_PULLUP);

  delay(1000);
  strcpy(ClientID,("MM-" + MacAddress).c_str());
  strcpy(ClientID_t,("MT-" + MacAddress).c_str()); 
  Serial.print("ClientID = ");
  Serial.println(ClientID);
  configWiFi();

  mqttClient.setClient(mqttWiFiClient);
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(dataInCallback);

  Serial.println(ClientID);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0);

  ++RTC_bootCount;
  Serial.println("Boot number: " + String(RTC_bootCount));
  Serial.println("Total accumulated time : " + String(RTC_totTime) + " seconds");

  Serial.println();

  readConfig();
  int cmp = (strcmp(UnitID, "0"));

  if (cmp == 0) {
    Serial.println(F("UnitID does = 0"));
    strcpy(UnitID, "N/A");
    saveConfig();
  }
    
    startTime = millis();  //Set start timer in milliseconds (1000/sec)
    startMQTT_Millis = millis(); // Used by MQTT routines
  
}