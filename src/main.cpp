#include "setup.h"


void loop(){
  runSensorState = digitalRead(RUN_SENSOR); // read the run sensor state (High is OFF and LOW is on)

  if (runSensorState == HIGH)
    goToSleep(); //HIGH means unit is NOT running

  if (digitalRead(AP_REQUEST) == LOW)
    openAP();
  
  if (!mqttClient.connected()) {
    mqttConnect();
  }
  mqttClient.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    mqttClient.publish("outTopic", msg);
  }
}