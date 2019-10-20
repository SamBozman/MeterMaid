#include "setup.h"


void loop(){
  runSensorState = digitalRead(RUN_SENSOR); // read the run sensor state (High is OFF and LOW is on)

  if (runSensorState == HIGH)
    goToSleep(); //HIGH means unit is NOT running

  if (digitalRead(AP_REQUEST) == LOW)
    openAP();
  //Serial.println("Loop Running :");
}