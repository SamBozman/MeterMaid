#include "setup.h"

void loop()
{
  /* Updates frequently */
  now = millis();

  runSensorState = digitalRead(RUN_SENSOR); // read the run sensor state (High is OFF and LOW is on)

  if (runSensorState == HIGH)
    goToSleep(); //HIGH means unit is NOT running

  if (digitalRead(AP_REQUEST) == LOW)
    openAP();

  delay(100);
  mqttClient.loop(); //NEEDED BY PubSubClient

  if (mqttClient.connected())
  {
    Last_Event2 = millis();
  }
  else
  {
    if (now - Last_Event2 >= EventInterval2)
    {
      mqttConnect();
      Last_Event2 = millis();
    }
  }
}