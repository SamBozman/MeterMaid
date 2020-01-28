#include "setup.h"

void loop() {
  now = millis();

  runSensorState = digitalRead(RUN_SENSOR);
  if (runSensorState == HIGH)
    goToSleep();

  if (digitalRead(AP_REQUEST) == LOW)
    openAP();

  delay(100);
  mqttClient.loop();

  if (mqttClient.connected()) {
    Serial.print(F("*"));
    Last_Event2 = millis();
  } else {
    if (now - Last_Event2 >= EventInterval2) {
      mqttConnect();
    }
  }
}