#pragma once
#include "declarations.h"

void updateConfig(byte *payload, unsigned int length)
{
  //This function handles the config update for topic = ClientID
  boolean cmp_flag = false; //Used to compare database config with saved config
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload);
  // Test if parsing succeeds.
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  Serial.println(F("deserializeJson() worked!! "));
  JsonObject arr0 = doc[0]; //Because an array was returned
  serializeJsonPretty(doc, Serial);
  Serial.println();
  //Extract the values
  const char *UID = arr0["UnitID"];
  int P_Mths = arr0["PMI_Months"];
  int P_Hrs = arr0["PMI_Hrs"];

  //Compare and set flag true if any one of the values is NOT equal
  if (!strcmp(UID, UnitID) == 0)
    cmp_flag = true;
  if (!(P_Mths == atoi(PMI_Months)))
    cmp_flag = true;
  if (!(P_Hrs == atoi(PMI_Hrs)))
    cmp_flag = true;

  //if flag is true then copy and save new values
  if (cmp_flag)
  {
    strcpy(UnitID, UID);
    //char *  itoa ( int value, char * str, int base );
    itoa(P_Mths, PMI_Months, 10);
    itoa(P_Hrs, PMI_Hrs, 10);
    saveConfig(); //Save new config
  }
  else
  {
    Serial.println(F("Database config was unchanged!")); //Value were the same
  }
}
//*********************************************************
void dataInCallback(char *topic, byte *payload, unsigned int length)
{
  //This function handles all CallBacks for all subscribed topics
  //and then routes them to the appropriate function
  if (strcmp(topic, ClientID) == 0)
  {
    Serial.println("Getting config file");
    updateConfig(payload, length);
  }
  else
  {
    Serial.println("CallBack Topic is " + String(topic));
  }
}
//*********************************************************
void checkPmiDue() //In Progress
{
  //This function is called when a PMI is due by Hrs or by Date
  Serial.println("Preparing Json file for Node-Red");
  StaticJsonDocument<256> json;

  //Transfer global variables to json document
  json["UnitID"] = UnitID;
  json["HourMeter"] = HourMeter;
  json["PMI_Months"] = PMI_Months;
  json["PMI_Hrs"] = PMI_Hrs;
  json["Last_PMI"] = Last_PMI;

  Serial.println();
  json.clear(); //Clear memory
}
//*********************************************************
void getAscTime(char *ptr_time)
{
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = gmtime(&rawtime);
  strftime(ptr_time, 11, "%F", timeinfo);
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
        strcpy(Last_PMI, json["Last_PMI"]);
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
  if (mqttClient.connect(ClientID))
  {
    Serial.println(F("connected"));
    mqttClient.subscribe(ClientID);            //Add subscribed topics here
    mqttClient.publish("noConfig", ClientID);  //Always send 'noConfig'
    mqttClient.publish("getConfig", ClientID); //Always pull the latest config file
    char *time_now(Now);                       //Set pointer to address of variable 'Now'
    getAscTime(time_now);                      //Puts date into char array 'Now'
    std::cout << "Date now is :" << Now << "\n";
  }
  else
  {
    Serial.print(F("RC error = :"));
    Serial.println(F("failed, rc="));
    Serial.print("State = :");
    Serial.println(mqttClient.state());
    Serial.println(F(" try again in 5 seconds"));
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
      json["Last_PMI"] = Last_PMI;

      serializeJson(json, configFile);   //Write data to config file
      serializeJsonPretty(json, Serial); //wite data to serial screen

      Serial.println();
      json.clear();       //Clear memory
      configFile.close(); //Save and close config file
      Serial.println(F("Config was saved to SPIFFS!"));
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
  int HM;                   //Used to convert char HourMeter into an int
  int a = totTime / 3600;   //This gives a the integer value of the division
  totTime = totTime % 3600; //This keeps the remainder for future saves
  HM = atoi(HourMeter);
  HM += a;
  //char *  itoa ( int value, char * str, int base );
  itoa(HM, HourMeter, 10);
  saveConfig();
}
//*********************************************************
void goToSleep()
{
  //If you saved every hour, 24 hours a day, every day you would take 11 years
  //before you reached the 100,000 save limit of the SPIFFS system

  stopTime = millis();
  runTime += (stopTime - startTime) / 1000; // last run time in seconds
  totTime += runTime;                       // add to accumilator

  if (totTime >= 3600) // 3600 seconds = 1 hour
  {
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
