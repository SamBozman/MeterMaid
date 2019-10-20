#pragma once
#include "functions.h"

void setup() //Runs once when first starting program (reset)
{
  Serial.begin(115200);
  pinMode(AP_REQUEST, INPUT_PULLUP);
  pinMode(RUN_SENSOR, INPUT_PULLUP);
  delay(1000); //Take some time to open up the Serial Monitor



  //Increment boot number and print it every reboot
  Serial.println("Boot number: " + String(bootCount));
  ++bootCount;

  runTime = 0; // reset runTime
  startTime = millis();
  Serial.println("Unit running, Starting Timer. ");
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0); //0 = Low (GND)
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("SUCCESS - Auto Connected during setup!)");
  
  
  //read configuration from FS json
  Serial.println("Attempt to read json/config file...");

  if (SPIFFS.begin(true))
  {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json"))
    {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");

      if (configFile)
      {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success())
        {
          Serial.println("\nparsed json");

          //char * strcpy ( char * destination, const char * source );

          strcpy(HourMeter, json["HourMeter"]);
          strcpy(PMI_Interval, json["PMI_Interval"]);
          strcpy(PMI_Extend, json["PMI_Extend"]);
          strcpy(WiFi_Retry, json["WiFi_Retry"]);
          
        }
        else
        {
          Serial.println("failed to load json config");
        }
      }
      else
      {
        Serial.println("failed to open /config.json file!");
      }
    }
    else
    {
      Serial.println("/config.json does not exist!");
    }
  }
  else
  {
    Serial.println("failed to mount FS");
  }
  SPIFFS.end();
  //end read
}