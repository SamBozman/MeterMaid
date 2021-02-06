#include "setup.h"

//********************************************************************
void loop() {
  runSensorState = digitalRead(RUN_SENSOR);
  check_status();
  currentMQTT_Millis = millis();
  // NOTE: WIFI on PC and DEVICE MUST BE THE SAME ###########################
  //if (WiFi.status() == WL_CONNECTED) {
    if (currentMQTT_Millis - startMQTT_Millis >= MQTT_Interval){
      Serial.println(F("Attempting MQTT connection!! "));
      if (mqttConnect()) {
        mqttClient.publish("getTime", ClientID_t);
        mqttClient.publish("insertESP", ClientID);
        mqttClient.publish("getConfig", ClientID);
        startMQTT_Millis = currentMQTT_Millis;
      }
    }
      // mqttClient.disconnect();

    // } else {
    //   if ((!strcmp("N/A", UnitID) == 0) &&
    //       (currentMQTT_Millis - startMQTT_Millis >= MQTT_Interval)) {
    //     Serial.println(F("Attempting MQTT connection!! "));
    //     if (mqttConnect()) {
    //       mqttClient.publish("getTime", ClientID_t);
    //       mqttClient.publish("getConfig", ClientID);
    //     }
    //     // mqttClient.disconnect();
    //     startMQTT_Millis = currentMQTT_Millis;
    //   }
    // }
  //}

  mqttClient.loop();
  delay(100); // Helps with WiFi Stability
  if (runSensorState == HIGH)
    goToSleep();

  if (digitalRead(AP_REQUEST) == LOW)
    openAP(); 
    // TODO setup AP_REQUEST to ask for  Mosquitto ip address
            
}
