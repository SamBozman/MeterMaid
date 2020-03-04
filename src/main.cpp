#include "setup.h"

//********************************************************************
void loop() {
  runSensorState = digitalRead(RUN_SENSOR);
  check_status();
  currentMQTT_Millis = millis();

  if (WiFi.status() == WL_CONNECTED) {
    if ((strcmp("N/A", UnitID) == 0) &&
        (currentMQTT_Millis - startMQTT_Millis >= MQTT_Interval)) {
      if (mqttConnect()) {
        mqttClient.publish("getTime", ClientID_t);
        mqttClient.publish("insertESP", ClientID);
        mqttClient.publish("getConfig", ClientID);
      }
      // mqttClient.disconnect();
      startMQTT_Millis = currentMQTT_Millis;
    } else {
      if ((!strcmp("N/A", UnitID) == 0) &&
          (currentMQTT_Millis - startMQTT_Millis >= MQTT_Interval)) {
        if (mqttConnect()) {
          mqttClient.publish("getTime", ClientID_t);
          mqttClient.publish("getConfig", ClientID);
        }
        // mqttClient.disconnect();
        startMQTT_Millis = currentMQTT_Millis;
      }
    }
  }

  mqttClient.loop();
  delay(100); // Helps with WiFi Stability
  if (runSensorState == HIGH)
    goToSleep();

  if (digitalRead(AP_REQUEST) == LOW)
    openAP();
}
