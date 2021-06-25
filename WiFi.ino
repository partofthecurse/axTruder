//Wifi-LogIn Manger
//based on the work of Jens Fleischer aka Flipsok

//HTML Files for the Log-In Page
const char HTML1[] PROGMEM = "<!DOCTYPE HTML><html lang='de'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
const char HTML2[] PROGMEM = "<style>button,input{padding:.5em 2em}body{background:#a9a9a9}</style><title>Login Manager</title></head><body><h2>SSID Passwort\
</h2><form action='/submit'><p><label>SSID:<br><input name='ssid' placeholder='Name vom Netzwerk' required></label></p><p><label>Passwort:<br><input \
name='passwort' pattern='[!-~]{8,64}' placeholder='PW vom Netzwerk' required></label></p><input type='submit' value='Absenden'></form></body></html>";
const char HTML3[] PROGMEM = "</head><body style ='background: #a9a9a9'><h3> Ihre Eingaben wurden erfolgreich übertragen. Der Server wird neu gestartet. </h3>";
const char HTML4[] PROGMEM = "<title>Verbunden</title></head><body style='background: #a9a9a9'><center><h2>Erfolgreich angemeldet!</h2><p>Super</p></center>";

const char* InputSSID = "ssid";
const char* InputPW = "passwort";
char ssid[33];
char password[65];

//----- WIFI SETUP -----
void setupWifi() {
  DEBUG_P("Function: setupWifi - started...");

  pinMode(LED_BUILTIN, OUTPUT);                       // OnBoardLed ESP32 Dev Module

  SPIFFS.begin(true);                                 //We use the SPIFFS to store the log in credentials in a txt file
  DEBUG_P("Spiffs Initialised");

  uint8_t i {0};                                      //We use i to count seconds until wifi-connection
  File file = SPIFFS.open("/login.txt", "r");
  if (file) {
    file.readBytesUntil('\n', ssid, sizeof(ssid));
    file.readBytesUntil('\n', password, sizeof(password));
    DEBUG_P("Got Log-In Credentials from File:");
    DEBUG_P(ssid);
    DEBUG_P(password);
    file.close();
  }

  WiFi.disconnect();
  DEBUG_P("Disconnecting...");
  WiFi.mode(WIFI_STA);                                //Station-Mode --> Esp32 connects to your Home Wifi
  DEBUG_P("Establishing Station Mode");  
  WiFi.begin(ssid, password);
  DEBUG_P("Try to Connect:");

  while (WiFi.status() != WL_CONNECTED) {             // While establishing connection, onboard led will be blinking
    pinMode(LED_BUILTIN, OUTPUT);                     // OnBoardLed Esp32 DevModule
    digitalWrite(LED_BUILTIN, 1);
    delay(500);
    digitalWrite(LED_BUILTIN, 0);
    delay(500);
    DEBUG_F(" % i sek\n", ++i);                       //For the SerialMonitor to display Time during connection process

    if (WiFi.status() != WL_CONNECTED && i > 20) {    // If router is not available for 20 seconds, we will switch to AP Mode
      digitalWrite(LED_BUILTIN, 1);                   // And turn on the onboard LED with no blinking.

      WiFi.disconnect();
      WiFi.mode(WIFI_AP);                             // Soft-Access-Point-Mode --> Access-Point Adress at http://192.168.4.1/
      DEBUG_P("\nTimeout: Starting Soft AP");

      if (WiFi.softAP("axTruder")) {
        DEBUG_F("Please connect to Wifi \"axTruder\"\nGo to IP \n\n", WiFi.softAPIP().toString().c_str());
        //inAP = true;
      }
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
   DEBUG_F("\nConnected to: %s\nEsp32 IP: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
   //DEBUG_P("Connection Successfull");
  }
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    //if we are in AP Mode we deliver the form in the top of this code
    if (WiFi.status() != WL_CONNECTED) {
      request->send(200, "text/html", (String)HTML1 + HTML2);
      DEBUG_P("Delivering Manager-Page");
    }
    //if we are in station mode we deliver the spiffs stored index html
    else {
      request->send(SPIFFS, "/index.html", String(), false, processor);
      DEBUG_P("Delivering Index from SPIFFS");
    }
  });

  //handle config -> in the form is a post request that gets activated once we fill out the form
  server.on("/submit", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam(InputSSID) && request->hasParam(InputPW)) { //if we filled out the form
      DEBUG_P("Received Inputparameters");
      File file = SPIFFS.open("/login.txt", "w");                 //we are opening our txt file with writing rights
      String inSSID;                                           //store the input to theese vars here
      String inPW;
      inSSID = request->getParam(InputSSID)->value();
      inPW = request->getParam(InputPW)->value();
      DEBUG_P(inSSID);
      DEBUG_P(inPW);
      file.printf("%s\n%s", inSSID.c_str(), inPW.c_str());    //and then put it in the text file
      DEBUG_P("Stored to File");
      file.close();
      DEBUG_P("Closed File");
      request->send(200, "text/html", (String)HTML1 + HTML3);       //then we deliver a success message
      DEBUG_P("Sending Success Page");
      delay(500);
      setupWifi();                                                    //and re-enter this function to start station mode
    }
    else {
      request->send(404, "text/plain", "OOpsie");
    }
  });

  DEBUG_P("Function: setupWifi... successfull");
}

void reConnect() {
  DEBUG_P("Function: reConnect... started ");                   // function te reconnect if in ap mode

  static unsigned long letzteMillis = 0;                       // cuz in case of energy break, esp is up faster
  unsigned long aktuelleMillis = millis();
  if (aktuelleMillis - letzteMillis >= 3e5) {                  // checks every 5 mins
    if (WiFi.status() != WL_CONNECTED) setupWifi();
    letzteMillis = aktuelleMillis;
  }
}
