/**
   ----------------------------------------------------------------------------
         _            _      _             _                _         _                      _                  _               _
        / /\        /_/\    /\ \          /\ \             /\ \      /\_\                   /\ \               /\ \            /\ \
       / /  \       \ \ \   \ \_\         \_\ \           /  \ \    / / /         _        /  \ \____         /  \ \          /  \ \
      / / /\ \       \ \ \__/ / /         /\__ \         / /\ \ \   \ \ \__      /\_\     / /\ \_____\       / /\ \ \        / /\ \ \
     / / /\ \ \       \ \__ \/_/         / /_ \ \       / / /\ \_\   \ \___\    / / /    / / /\/___  /      / / /\ \_\      / / /\ \_\
    / / /  \ \ \       \/_/\__/\        / / /\ \ \     / / /_/ / /    \__  /   / / /    / / /   / / /      / /_/_ \/_/     / / /_/ / /
   / / /___/ /\ \       _/\/__\ \      / / /  \/_/    / / /__\/ /     / / /   / / /    / / /   / / /      / /____/\       / / /__\/ /
  / / /_____/ /\ \     / _/_/\ \ \    / / /          / / /_____/     / / /   / / /    / / /   / / /      / /\____\/      / / /_____/
  / /_________/\ \ \   / / /   \ \ \  / / /          / / /\ \ \      / / /___/ / /     \ \ \__/ / /      / / /______     / / /\ \ \
  / / /_       __\ \_\ / / /    /_/ / /_/ /          / / /  \ \ \    / / /____\/ /       \ \___\/ /      / / /_______\   / / /  \ \ \
  \_\___\     /____/_/ \/_/     \_\/  \_\/           \/_/    \_\/    \/_________/         \/_____/       \/__________/   \/_/    \_\/

   ----------------------------------------------------------------------------
   © ESP32 Asyncwebserver to Control a DIY Extruder for Plastic and DIY Filamentproduction.
   Most of the Controls are in the Websocket Tab. The Graphs use XMLHTTP Requests for updating, the buttons use websocket and JSON
   Features:
      - ESP32 µC
      - L298N Motordriver
      - Various Thermistors (at the Moment only NTC)
      - Digital Caliper ReadOut based on Work of MGX3D - See https://github.com/MGX3D/EspDRO for Details - i included stl files for the sensor-mod
      - using highcharts js to visualise temperature and diameter in graphs
      - Preheat Protection (do not rotate untill nozzle temp is reached)
      - MQTT Data Logging (See MQTT Tab for Topics)
      - PID Temperature Control - you should adapt values for your hardware
      - PID Diameter Control - Adapt to your Motor and Gear Numbers
   ----------------------------------------------------------------------------
   To Do
   - PID Temperature Control testing and optimizing controller values
   - mDNS
   - captive Portal
   - OTA Updates
   - Wifi Multi
   - thermal runaway protection
   - integrate JSON better
   - add support more Thermistors
   - manual rotation time setting in ui
   - speed presets in material dropdown

   © 2020 Daniel Strohbach
   ----------------------------------------------------------------------------
*/

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>


#define DEBUGGING                     // debugging needed?

#ifdef DEBUGGING
#define DEBUG_B(...) Serial.begin(__VA_ARGS__)
#define DEBUG_P(...) Serial.println(__VA_ARGS__)
#define DEBUG_F(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_B(...)
#define DEBUG_P(...)
#define DEBUG_F(...)
#endif

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
//Create AsyncWebsocket Object on /ws
AsyncWebSocket ws("/ws");

//some working variables
int nozzleTempSP = 0;
int nozzleTempOffsetSP = 0;
float diameterSP = 1.75;
float diameterOffsetSP = 0;
float manualRotationTime = 3000; // 3 Seconds for manual rotation as default


const char* axTruderState = "idle";

StaticJsonDocument<200> jsonBuffer;

//Variables for interaction with user input on webinterface (i will try to get rid of this with websockets)
//const char* TEMP_INPUT = "tempInput";
//const char* TEMP_OFFSET_INPUT = "tempOffsetInput";
//const char* DIAMETER_INPUT = "diameterInput";
//const char* DIAMETER_OFFSET_INPUT = "diameterOffsetInput";

//vars for the machine state logic
bool heaterOn = false; // when the heater is turned on
bool manualOk = false; // when nozzle is hot but the target temp is not reached yet -> unlocks ui buttons for rotation
bool nozzleOk = false; // when the target temp is reached -> unlocks rotation
bool augerOn = false; // when we are rotating -> unlocks spoolwinder
bool extruding = false; //when we are in extrusion mode
bool spoolOn = false;

//--- Timer ---
unsigned long previousMillisMes; //alte Zeit
unsigned long previousMillisPub; //alte Zeit

const unsigned long messIntervall = 2000;
const unsigned long publishIntervall = 5000;

//--- Template Processor -> looks for all Placeholders and fills them with actual values
String processor(const String& var) {
  Serial.println(var);

  if (var == "AXTRUDERSTATE") {
    return String(axTruderState); //gets last Stored Value
  }

  if (var == "NOZZLE_TEMPERATURE_SP") {
    return String(nozzleTempSP); //gets last Stored Value
  }

  else if (var == "NOZZLE_TEMPERATURE") {
    return String(readNTC()); //reads temp once
  }

  else if (var == "DIAMETER_SP") {
    return String(diameterSP); //gets last Stored Value
  }

  else if (var == "DIAMETER") {
    return String(readDRO()); //reads diameter once
  }

  else if (var == "DIAMETER_OFFSET_SP") {
    return String(diameterOffsetSP); //gets last Stored Value
  }

  else if (var == "NOZZLE_TEMPERATURE_OFFSET_SP") {
    return String(nozzleTempOffsetSP); //gets last Stored Value
  }

  return String();
}

//--- ARDUINO SETUP---
void setup() {
  // Serial port for debugging purposes
  DEBUG_B(115200);

  //Initialise SPIFFS (See SPIFFS Tab)
  setupSPIFFS();

  //Initialise NTC Stuff (See NTC Tab)
  setupNTC();

  //Initialise Auger (See Auger Tab)
  setupAuger();

  //Initialise PID Stuff
  setupHeater();

  //Initialise DRO
  setupDRO();

  //Initialise Wifi Stuff (See Wifi Tab)
  setupWifi();

  //Initialise MQTT
  setupMQTT();

  //Initialise Websocket
  initWebSocket();

  //Serving the CSS File
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  //Serving the JS File
  server.on("/javascript.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/javascript.js", "text/javascript");
  });

  //Serving the index if we go there manually (in ap mode e.g.)
  server.on("/index", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  //Serving the login txt file from wifi manager if we go there manually (in ap mode e.g.)
  server.on("/login", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/login.txt", "text/plain");
  });

    //Serving the login txt file from wifi manager if we go there manually (in ap mode e.g.)
  server.on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.png", "image/png");
  });

  // Start server
  server.begin();
}

//--- LOOP ---
void loop() {

  //we want to measure the current nozzle Temperature and Diameter in a certain interval -> this is only for the Charts, for control we measure more often.
  if (millis() - previousMillisMes >= messIntervall) {
    previousMillisMes = millis();
    notifyClient("nozzleTemp", readNTC());
    notifyClient("diameter", readDRO());
  }

  //publish to MQTT server
  if (millis() - previousMillisPub >= publishIntervall) {
    previousMillisPub = millis();
    publishMQTT();
  }

  //if we pressed the preheat button, we can heat the nozzle - this is starting the pid controll of the heater
  if (heaterOn) {
    heatNozzle();
    checkNozzle();
  }
  //if cooldown is pressed the bool will be false and the loop will not be entered
  //then if we are in extrusion mode, we want to constantly check for the correct filament diameter
  if (extruding){
    controlDiameter();
  }
}
