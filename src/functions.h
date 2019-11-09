#pragma once
#include "declarations.h"

//*********************************************************

void dataInCallback(char *topic, byte *payload, unsigned int length)
{

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
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
    //    // Create a random client ID
    //    String clientId = "ESP8266Client-";
    //    clientId += String(random(0xffff), HEX);
    //    // Attempt to connect
    //    if (client.connect(clientId.c_str())) {
    if (mqttClient.connect(ClientID))
    {
      Serial.println("connected");

      // Once connected, publish an announcement...(REMOVE FROM FINAL)
      mqttClient.publish("outTopic", "hello world from mqttConnect function");

      // PLACE SUNSCRIBED TOPICS HERE
      mqttClient.subscribe("inTopic");
      mqttClient.subscribe(ClientID);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
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

//*********************************************************
void saveConfig()
{
  Serial.println("saving config");
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();

  json["HourMeter"] = HourMeter;
  json["PMI_Interval"] = PMI_Interval;
  json["PMI_Extend"] = PMI_Extend;
  json["WiFi_Retry"] = WiFi_Retry;

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

  shouldSaveConfig = false;
  //end write
}

//*********************************************************
void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
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
    addToHM();

  Serial.print("Total time = ");
  Serial.print(totTime);
  Serial.println(" seconds");

  Serial.println("Going to sleep now");
  pinMode(RUN_SENSOR, INPUT_PULLUP);
  esp_deep_sleep_start();
}

//*********************************************************
void openAP()
{
  // wifiManager.setTimeout(120); //sets timeout until configuration portal gets turned off

  //Triggered when 'Save' button is pressed on AP WebServer
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //id/name, placeholder/prompt, default, length
  WiFiManagerParameter custom_HourMeter("HM", "HourMeter", HourMeter, 8);
  WiFiManagerParameter custom_PMI_Interval("PI", "PMI_Interval", PMI_Interval, 6);
  WiFiManagerParameter custom_PMI_Extend("PE", "PMI_Extend", PMI_Extend, 4);
  WiFiManagerParameter custom_WiFi_Retry("WR", "WiFi_Retry", WiFi_Retry, 4);

  wifiManager.addParameter(&custom_HourMeter);
  wifiManager.addParameter(&custom_PMI_Interval);
  wifiManager.addParameter(&custom_PMI_Extend);
  wifiManager.addParameter(&custom_WiFi_Retry);

  //set static ip
  wifiManager.setSTAStaticIPConfig(IPAddress(10, 0, 1, 99), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));

  //reset settings - for testing
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

  //Retrive changed values from Web Server
  strcpy(HourMeter, custom_HourMeter.getValue());
  strcpy(PMI_Interval, custom_PMI_Interval.getValue());
  strcpy(PMI_Extend, custom_PMI_Extend.getValue());
  strcpy(WiFi_Retry, custom_WiFi_Retry.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig)
  {
    saveConfig();
  }
}