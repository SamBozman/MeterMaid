#include "setup.h"

void loop() {
  runSensorState = digitalRead(RUN_SENSOR);
  // put your main code here, to run repeatedly
  check_status();
  // if (WiFi.status() == WL_CONNECTED)

  // if (mqttClient.connected()) {
  //   Serial.print(F("*"));
  //   Last_Event2 = millis();
  // } else {
  //   if (now - Last_Event2 >= EventInterval2) {
  //     mqttConnect();
  //   }
  // }
  mqttClient.loop();
  delay(100); // Helps with WiFi Stability
              // disconnect() //Disconnects the client.

  //#######################################
  if (runSensorState == HIGH)
    goToSleep();

  if (digitalRead(AP_REQUEST) == LOW)
    openAP();
}