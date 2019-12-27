#pragma once
#include "declarations.h"

//*********************************************************
// int conCat() //Example of working concatenator
// {
//   //Needs work to catch errors
//   char str[80];
//   strcpy(str, PMI_Extend);
//   strcat(str, PMI_Hrs);
//   strcat(str, " is ");
//   strcat(str, "concatenated.");
//   puts(str);
//   return 0;
// }

//*********************************************************
void dataInCallback(char *topic, byte *payload, unsigned int length)
{

  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }

  Serial.println();
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
      // PLACE SUNSCRIBED TOPICS HERE
      //mqttClient.subscribe("inTopic");
      mqttClient.subscribe(ClientID);
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
int charToInt(const char *numArray, int *value)
{
  int n = 0;
  return sscanf(numArray, "%d%n", value, &n) > 0 /* integer was converted */
         && numArray[n] == '\0';                 /* all input got consumed */
}

//*********************************************************
void saveConfig()
{
  Serial.println(F("saving config"));
  //DynamicJsonBuffer jsonBuffer;
  //JsonObject &json = jsonBuffer.createObject();
  //const int capacity = JSON_OBJECT_SIZE(5);

  //serializeJson(json, file);
  //file.close();

  if (SPIFFS.begin())
  {
    Serial.println(F("mounted file system"));

    File configFile = SPIFFS.open("/conf.json", "w");

    if (configFile)
    {
      Serial.println("writing config file");
      StaticJsonDocument<256> json;
      JsonObject object = json.to<JsonObject>();

      object["UnitID"] = "N/A";
      object["HourMeter"] = HourMeter;
      object["PMI_Months"] = PMI_Months;
      object["PMI_Hrs"] = PMI_Hrs;
      object["Date"] = Date;

      serializeJson(json, configFile);
      serializeJsonPretty(json, Serial);

      Serial.println();
      configFile.close();

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
void readConfig()
{
  //   char UnitID[7];//Unit number
  // char HourMeter[6] = "0";//Keeps track of hours of operation
  // char PMI_Months[3] = "12";//Number of Months to wait to trigger a PMI
  // char PMI_Hrs[4] = "250";//Number of hours to wait until triggering a PMI
  // char Date[11]; //Date PMI completed

  if (SPIFFS.begin(true))
  {
    Serial.println("Opened SPIFFS file system");
    if (SPIFFS.exists("/conf.json"))
    {
      //file exists, reading and loading
      Serial.println(F("reading config file"));
      File configFile = SPIFFS.open("/conf.json", "r");
      if (configFile)
      {
        Serial.println(F("opened config file"));
        // Allocate a buffer to store contents of the file.
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        Serial.print(F("configFile = "));
        Serial.println(buf.get());

        DynamicJsonDocument json(255);

        DeserializationError error = deserializeJson(json, configFile);

        // Test if parsing succeeds.
        if (error)
        {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.c_str());
          configFile.close();
          SPIFFS.end();
          return;
        }
        Serial.print(F("deserializeJson() worked!! "));
        // extract the data
        //JsonObject object = doc.as<JsonObject>();
        //const char *world = object["hello"];

        // Serial.println(F("\nparsed json");
        // strcpy(UnitID, json["UnitID"]);
        // strcpy(HourMeter, json["HourMeter"]);
        // strcpy(PMI_Months, json["PMI_Months"]);
        // strcpy(PMI_Hrs, json["PMI_Hrs"]);
        // strcpy(PMI_Extend, json["PMI_Extend"]);
      }
    }
  }
  else
  {
    Serial.println(F("failed to mount FS"));
  }

  SPIFFS.end();
}

//*********************************************************
void addToHM()
{
  int a = totTime / 3600;
  totTime = totTime % 3600;
  HM = atoi(HourMeter);
  HM += a;
  itoa(HM, HourMeter, 10);
  //saveConfig(); #####################################################
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
    Serial.print(totTime);         //#######################################
    Serial.println(F(" seconds"));    //#################################
    delay(2000);

    addToHM();
  }

  Serial.println("Going to sleep now");
  pinMode(RUN_SENSOR, INPUT_PULLUP);
  esp_deep_sleep_start();
}

//*********************************************************
void openAP()
{
  //sets timeout until configuration portal gets turned off
  wifiManager.setTimeout(120);
  //Triggered when 'Save' button is pressed on AP WebServer

  //set static ip
  wifiManager.setSTAStaticIPConfig(IPAddress(10, 0, 1, 99), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));

  //reset settings - uncomment below for testing
  //wifiManager.resetSettings();

  //defaults to 8%
  //wifiManager.setMinimumSignalQuality();

  if (!wifiManager.startConfigPortal("OnDemandAP"))
  {
    Serial.println(F("failed to connect and hit timeout"));
    delay(1000);
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
