#include "setup.h"

void loop()
{
  /* Updates frequently */
  unsigned long now = millis();

  runSensorState = digitalRead(RUN_SENSOR); // read the run sensor state (High is OFF and LOW is on)

  if (runSensorState == HIGH)
    goToSleep(); //HIGH means unit is NOT running

  if (digitalRead(AP_REQUEST) == LOW)
    openAP();

  /* This is my event_1 */
  // if (now - Last_E1 >= E1)
  // {
  //   Serial.println("Timer for event 1 just fired: ");
  //   Last_E1 = now;
  // }

  if (mqttClient.connected())
  {
    Last_E2 = now;
  }
  else
  {
    if (now - Last_E2 >= E2)
    {
      mqttConnect();
      now = millis();
      Last_E2 = now;
    }
  }
  delay(100);
  mqttClient.loop(); //NEEDED BY PubSubClient
}