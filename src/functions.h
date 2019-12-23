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
  StaticJsonBuffer<200> jsonBuffer;

  std::string str;
 

  Serial.print("Message arrived [");
  Serial.println(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    str += (char)payload[i];
  }
  
  char cstr[str.size() + 1];
  strcpy(cstr, str.c_str());

  Serial.println();
  Serial.println(cstr);
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
      Serial.println("connected");
      // PLACE SUNSCRIBED TOPICS HERE
      //mqttClient.subscribe("inTopic");
      mqttClient.subscribe(ClientID);
    }
    else
    {
      Serial.print("RC error = :");
      Serial.println("failed, rc=");
      Serial.print("State = :");
      Serial.println(mqttClient.state());
      Serial.println(" try again in 5 seconds");
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
void readConfig()
{
  if (SPIFFS.begin(true))
  {
    Serial.println("Opened SPIFFS file system");
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

        if (json.success())
        {
          json.printTo(Serial);
          Serial.println("\nparsed json");
          strcpy(UnitID, json["UnitID"]);
          strcpy(HourMeter, json["HourMeter"]);
          strcpy(PMI_Months, json["PMI_Months"]);
          strcpy(PMI_Hrs, json["PMI_Hrs"]);
          strcpy(PMI_Extend, json["PMI_Extend"]);
        }
      }
    }
  }
  else
  {
    Serial.println("failed to mount FS");
  }
  SPIFFS.end();
}

//*********************************************************
void saveConfig()
{
  Serial.println("saving config");
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();

  json["UnitID"] = UnitID;
  json["HourMeter"] = HourMeter;
  json["PMI_Months"] = PMI_Months;
  json["PMI_Hrs"] = PMI_Hrs;
  json["PMI_Extend"] = PMI_Extend;

  if (SPIFFS.begin())
  {
    Serial.println("mounted file system");

    File configFile = SPIFFS.open("/config.json", "w");
    if (configFile)
    {
      Serial.println("writing config file");
      json.prettyPrintTo(Serial);
      json.printTo(configFile);
      configFile.close();
      SPIFFS.end();
      //ESP.restart();
    }
  }
  else
  {
    Serial.println("failed to mount FS");
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
  saveConfig();
}

//*********************************************************
void goToSleep()
{
  stopTime = millis();
  runTime += (stopTime - startTime) / 1000; // last run time in seconds
  totTime += runTime;                       // add to accumilator

  if (totTime >= 3600)
  {
    Serial.print("Total time = "); //###############################
    Serial.print(totTime);         //#######################################
    Serial.println(" seconds");    //#################################
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
    Serial.println("failed to connect and hit timeout");
    delay(1000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    Serial.println("Restarting ESP");
  }
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
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
  Serial.print("The chip ID is ");
  Serial.println(ClientID);
}
