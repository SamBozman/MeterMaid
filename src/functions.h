#pragma once
#include "declarations.h"

//*********************************************************
void checkPmiDue() {
  // char strTime[11] = "2020-01-27"; // Last_PMI
  const long int secondsInaDay = 86400; // Number of seconds in a day
  const long int minDate = 1264695154;
  const int PMI_days = 30 * atoi(PMI_Months);
  unsigned long int unixLastPmi;
  int int_HourMeter = atoi(HourMeter);
  int int_PMI_Hrs = atoi(PMI_Hrs);

  // check for PMI due by months
  char *ptr_strTime = Last_PMI;
  struct tm tm;
  time_t ts;
  memset(&tm, 0, sizeof(struct tm));
  strptime(ptr_strTime, "%Y-%m-%d", &tm);
  ts = mktime(&tm);
  unixLastPmi = (int)ts;

  int daysSinceLastPMI = (CurrentTime - unixLastPmi) / secondsInaDay;
  cout << "Days since last pmi = " << daysSinceLastPMI << endl;
  if (daysSinceLastPMI > PMI_days) {
    cout << "PMI DUE by Months" << endl;
  }

  // check for PMI due by Hourmeter
  cout << "HourMeter = " << HourMeter << endl;
  cout << "int_PMI_Hrs = " << int_PMI_Hrs << endl;
  if (int_HourMeter > int_PMI_Hrs) {
    cout << "PMI DUE by Hourmeter" << endl;
  }
}
//*********************************************************
void updateConfig(byte *payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  Serial.println(F("deserializeJson() worked!! "));
  JsonObject arr0 = doc[0];
  serializeJsonPretty(doc, Serial);
  Serial.println();
  // Extract the values
  const char *UID = arr0["UnitID"];
  int P_Mths = arr0["PMI_Months"];
  int P_Hrs = arr0["PMI_Hrs"];
  int U_Hrs = arr0["Preset_Hrs"];
  const char *P_UH = arr0["Last_PMI"];

  boolean cmp_flag = false;
  if (!(U_Hrs <= -1)) {
    Serial.print(F("Preset_Hrs was "));
    Serial.println(U_Hrs);
    itoa(U_Hrs, HourMeter, 10);
    cmp_flag = true;
  }
  if (!strcmp(UID, UnitID) == 0)
    cmp_flag = true;
  if (!(P_Mths == atoi(PMI_Months)))
    cmp_flag = true;
  if (!(P_Hrs == atoi(PMI_Hrs)))
    cmp_flag = true;
  if (!strcmp(P_UH, Last_PMI) == 0)
    cmp_flag = true;

  if (cmp_flag) {
    strcpy(UnitID, UID);
    // char *  itoa ( int value, char * str, int base );
    itoa(P_Mths, PMI_Months, 10);
    itoa(P_Hrs, PMI_Hrs, 10);
    strcpy(Last_PMI, P_UH);
    Serial.println(F("Database config was changed!"));
    mqttClient.publish("confirmConfig", UnitID);
    saveConfig();
  } else {
    Serial.println(F("Database config was unchanged!"));
  }
  checkPmiDue();
  Serial.println();
}
//*********************************************************
void dataInCallback(char *topic, byte *payload, unsigned int length) {
  // getConfig
  if (strcmp(topic, ClientID) == 0) {
    Serial.println("Getting config file");
    updateConfig(payload, length);
  } else {
    // get timestamp
    if (strcmp(topic, ClientID_t) == 0) {
      cout << "Topic = ClientID_t " << endl;
      if (length == (13)) {
        char time[length - 2];
        for (int i = 0; i < length - 3; i++) {
          time[i] = (char)payload[i];
        }
        time[length - 3] = '\0';
        CurrentTime = strtol(time, NULL, 10);
        cout << "Current UnixTimestamp is: " << CurrentTime << endl;
      }
    }
  }
}

//*********************************************************
void readConfig() {
  if (SPIFFS.begin(true)) {
    Serial.println("Opened SPIFFS file system");
    if (SPIFFS.exists("/config.json")) {
      Serial.println(F("reading config file"));
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println(F("opened config file"));

        StaticJsonDocument<256> json;

        DeserializationError error = deserializeJson(json, configFile);
        if (error) {
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
      } else {
        Serial.println(F("Could not read config file"));
      }
    } else {
      Serial.println(F("Config file does not exist"));
    }
  } else {
    Serial.println(F("failed to mount FS"));
  }

  SPIFFS.end();
}

//*********************************************************
void mqttConnect() {
  if (mqttClient.connect(ClientID)) {
    Serial.println(F("connected"));
    mqttClient.subscribe(ClientID);
    mqttClient.subscribe(ClientID_t);

    mqttClient.publish("getTime", ClientID_t);
    mqttClient.publish("noConfig", ClientID);
    mqttClient.publish("getConfig", ClientID);

  } else {
    Serial.print(F("RC error = :"));
    Serial.println(F("failed, rc="));
    Serial.print("State = :");
    Serial.println(mqttClient.state());
    Serial.println(F(" try again in 5 seconds"));
  }
}
//*********************************************************
void saveConfig() {
  Serial.println(F("saving config"));

  if (SPIFFS.begin()) {
    Serial.println(F("mounted file system"));
    if (SPIFFS.exists("/config.json")) {
      Serial.println("removing config.json file");
      SPIFFS.remove("/config.json");
    }

    File configFile = SPIFFS.open("/config.json", "w");

    if (configFile) {
      Serial.println("writing config file");
      StaticJsonDocument<256> json;

      json["UnitID"] = UnitID;
      json["HourMeter"] = HourMeter;
      json["PMI_Months"] = PMI_Months;
      json["PMI_Hrs"] = PMI_Hrs;
      json["Last_PMI"] = Last_PMI;

      serializeJson(json, configFile);
      serializeJsonPretty(json, Serial);

      Serial.println();
      json.clear();
      configFile.close();
      Serial.println(F("Config was saved to SPIFFS!"));
      SPIFFS.end();
    }
  } else {
    Serial.println(F("failed to mount FS"));
  }
}
//*********************************************************
void addToHM() {
  int HM;
  int a = totTime / 3600;
  totTime = totTime % 3600;
  HM = atoi(HourMeter);
  HM += a;
  // char *  itoa ( int value, char * str, int base );
  itoa(HM, HourMeter, 10);
  saveConfig();
}
//*********************************************************
void goToSleep() {
  // If you saved every hour, 24 hours a day, every day you would take 11 years
  // before you reached the 100,000 save limit of the SPIFFS system
  stopTime = millis();
  runTime += (stopTime - startTime) / 1000;
  totTime += runTime;

  if (totTime >= 3600) // 1 hour
  {
    addToHM();
  }

  Serial.println("Going to sleep now");
  pinMode(RUN_SENSOR, INPUT_PULLUP);
  esp_deep_sleep_start();
}
//*********************************************************
void openAP() // TODO: Test this function
{
  wifiManager.setTimeout(120); // 2 minutes
  // FIXME: See if static ip address is necessary or??
  wifiManager.setSTAStaticIPConfig(IPAddress(10, 0, 1, 99),
                                   IPAddress(10, 0, 1, 1),
                                   IPAddress(255, 255, 255, 0));

  // wifiManager.resetSettings();
  if (!wifiManager.startConfigPortal("OnDemandAP")) {
    Serial.println(F("failed to connect and hit timeout"));
    delay(500);
    ESP.restart();
    Serial.println(F("Restarting ESP"));
  }
  Serial.println(F("WiFi Connected!"));
  ESP.restart();
}
//*********************************************************
void createChipID() {
  uint64_t chipid;
  chipid = ESP.getEfuseMac();
  snprintf(ClientID, 11, "E-%X", (uint32_t)chipid);     // ClientID
  snprintf(ClientID_t, 13, "E-%X/t", (uint32_t)chipid); // ClientID_t
}
