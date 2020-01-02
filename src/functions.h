#pragma once
#include "declarations.h"

//*********************************************************
void dataInCallback(char *topic, byte *payload, unsigned int length)
{
  // TODO: Need to handle various incoming messages here
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.println(F("] "));
  Serial.print("Length of payload = ");
  Serial.println(length);

  // for (int i = 0; i < length; i++)
  // {
  //   Serial.print((char)payload[i]);
  // }
  Serial.println();

  StaticJsonDocument<256> json;
  DeserializationError error = deserializeJson(json, payload);
  // Test if parsing succeeds.
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    json.clear();
    return;
  }
  Serial.println(F("deserializeJson() worked!! "));
  serializeJsonPretty(json, Serial);
  Serial.println();

  json.clear();
}
//*********************************************************
void readConfig()
{
  if (SPIFFS.begin(true))
  {
    Serial.println("Opened SPIFFS file system");
    if (SPIFFS.exists("/config.json"))
    {
      //file exists, reading and loading
      Serial.println(F("reading config file"));
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile)
      {
        Serial.println(F("opened config file"));

        StaticJsonDocument<256> json;

        DeserializationError error = deserializeJson(json, configFile);

        // Test if parsing succeeds.
        if (error)
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.c_str());
          configFile.close();
          json.clear();
          SPIFFS.end();
          return;
        }
        Serial.println(F("deserializeJson() worked!! "));
        serializeJsonPretty(json, Serial);
        Serial.println();

        strcpy(UnitID, json["UnitID"]);
        strcpy(HourMeter, json["HourMeter"]);
        strcpy(PMI_Months, json["PMI_Months"]);
        strcpy(PMI_Hrs, json["PMI_Hrs"]);
        strcpy(Date, json["Date"]);
        configFile.close();
        json.clear();
      }
      else
      {
        Serial.println(F("Could not read config file"));
      }
    }
    else
    {
      Serial.println(F("Config file does not exist"));
    }
  }
  else
  {
    Serial.println(F("failed to mount FS"));
  }

  SPIFFS.end();
}
//*********************************************************
void mqttConnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect(ClientID))
    {
      Serial.println(F("connected"));
      mqttClient.subscribe(ClientID); //We are subscribed to our own ClientID
      // TODO: Add any other topics here that we need to sudcribe to
    }
    else
    {
      Serial.print(F("RC error = :"));
      Serial.println(F("failed, rc="));
      Serial.print("State = :");
      Serial.println(mqttClient.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//*********************************************************
void saveConfig()
{
  Serial.println(F("saving config"));

  if (SPIFFS.begin())
  {
    Serial.println(F("mounted file system"));
    if (SPIFFS.exists("/config.json"))
    {
      // Delete existing file, otherwise the configuration is appended to the file
      Serial.println("removing config.json file");
      SPIFFS.remove("/config.json");
    }
    //Now open(Create) NEW conf.json file
    File configFile = SPIFFS.open("/config.json", "w");

    if (configFile) //If a NEW config file was successfully created then ...
    {
      Serial.println("writing config file");
      StaticJsonDocument<256> json;

      //Transfer global variables to json document
      json["UnitID"] = UnitID;
      json["HourMeter"] = HourMeter;
      json["PMI_Months"] = PMI_Months;
      json["PMI_Hrs"] = PMI_Hrs;
      json["Date"] = Date;

      serializeJson(json, configFile);   //Write data to config file
      serializeJsonPretty(json, Serial); //wite data to serial screen

      Serial.println();
      json.clear();       //Clear memory
      configFile.close(); //Save and close config file

      SPIFFS.end();
      //ESP.restart();
    }
  }
  else
  {
    Serial.println(F("failed to mount FS"));
  }
}

//*********************************************************
void addToHM()
{
  int a = totTime / 3600;
  totTime = totTime % 3600;
  HM = atoi(HourMeter);
  HM += a;
  itoa(HM, HourMeter, 10);
  //saveConfig(); // TODO:  Remove comment lines when finished testing
}

//*********************************************************
void goToSleep()
{
  stopTime = millis();
  runTime += (stopTime - startTime) / 1000; // last run time in seconds
  totTime += runTime;                       // add to accumilator

  if (totTime >= 3600)
  {
    Serial.print(F("Total time = ")); //###############################
    Serial.print(totTime);            //#######################################
    Serial.println(F(" seconds"));    //#################################
    delay(2000);

    addToHM();
  }

  Serial.println("Going to sleep now");
  pinMode(RUN_SENSOR, INPUT_PULLUP);
  esp_deep_sleep_start();
}

//*********************************************************
void openAP() //TODO: Test this function
{
  //sets timeout until configuration portal gets turned off
  wifiManager.setTimeout(120);
  //Triggered when 'Save' button is pressed on AP WebServer

  //FIXME: See if static ip address is necessary or??
  wifiManager.setSTAStaticIPConfig(IPAddress(10, 0, 1, 99), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));

  //reset settings - uncomment below for testing
  //wifiManager.resetSettings();

  // Set minimum signal strength - defaults to 8%
  //wifiManager.setMinimumSignalQuality();

  if (!wifiManager.startConfigPortal("OnDemandAP"))
  {
    Serial.println(F("failed to connect and hit timeout"));
    delay(500);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    Serial.println(F("Restarting ESP"));
  }
  //if you get here you have connected to the WiFi
  Serial.println(F("connected...yeey :)"));
  ESP.restart();
}
//*********************************************************
void createChipID()
{
  uint16_t chip;
  uint64_t chipid;
  chipid = ESP.getEfuseMac(); //The chip ID is essentially its MAC address(length: 6 bytes).
  chip = (uint16_t)(chipid >> 32);
  snprintf(ClientID, 23, "ESP-%04X%08X", chip, (uint32_t)chipid);
  Serial.print(F("The chip ID is "));
  Serial.println(ClientID);
}
