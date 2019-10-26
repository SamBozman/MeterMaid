#pragma once

#include <FS.h> //this needs to be first, or it all crashes and burns...
#include "SPIFFS.h"
#include <Arduino.h>
// #if defined(ESP8266)
// #include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
// #else
#include <WiFi.h> //https://github.com/esp8266/Arduino
//#endif

//needed for library
#include <DNSServer.h>
// #if defined(ESP8266)
// #include <ESP8266WebServer.h>
// #else
#include <WebServer.h>
//#endif
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager/tree/development
#include <PubSubClient.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

