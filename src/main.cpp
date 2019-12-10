#include "setup.h"

void loop()
{
  runSensorState = digitalRead(RUN_SENSOR); // read the run sensor state (High is OFF and LOW is on)

  if (runSensorState == HIGH)
    goToSleep(); //HIGH means unit is NOT running

  if (digitalRead(AP_REQUEST) == LOW)
    openAP();

  if (!mqttClient.connected())
  {
    mqttConnect();
  }
  delay(100);
  mqttClient.loop(); //NEEDED BY PubSubClient

  //############  TEMPORARY FOR TESTING ###########################
  long now = millis();
  if (now - lastMsg > 20000)
  { //Publish every 20 seconds
    lastMsg = now;
    ++value;

    Serial.print("ESP32 - to node ");
    Serial.println(ClientID); //Prints out unique ChipID (MAC address)
    mqttClient.publish("outTopic", ClientID);
    Serial.println();
    }
  //############  end  of TEMPORARY FOR TESTING ###########################
}