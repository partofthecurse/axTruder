//-----------Increase - Decrease Buttons-----
//Small + - Buttons on Temperature Setpoint
//  if (MessageBuffer == "decreaseTemp") {
//    nozzleTempSP -= 1;
//    DEBUG_F("Function: decreaseTemp - New NozzleTemp: ");
//    DEBUG_P(nozzleTempSP);
//    notifyClient("nozzleTempSP", String(nozzleTempSP));;
//  }
//  if (MessageBuffer == "increaseTemp") {
//    nozzleTempSP += 1;
//    DEBUG_F("Function: increaseTemp - New NozzleTemp: ");
//    DEBUG_P(nozzleTempSP);
//    notifyClient("nozzleTempSP", String(nozzleTempSP));
//  }
//  //Small + - Buttons on Temperature Offset Setpoint
//  if (MessageBuffer == "decreaseTempOffset") {
//    nozzleTempOffsetSP -= 1;
//    DEBUG_F("Function: decreaseTempOffset - New NozzleTempOffset: ");
//    DEBUG_P(nozzleTempOffsetSP);
//    notifyClient("nozzleTempOffsetSP", String(nozzleTempOffsetSP));
//  }
//  if (MessageBuffer == "increaseTempOffset") {
//    nozzleTempOffsetSP += 1;
//    DEBUG_F("Function: increaseTempOffset - New NozzleTempOffset: ");
//    DEBUG_P(nozzleTempOffsetSP);
//    notifyClient("nozzleTempOffsetSP", String(nozzleTempOffsetSP));
//  }
//  //Small + - Buttons on Diameter Setpoint
//  if (MessageBuffer == "decreaseDiameter") {
//    diameterSP -= 0.01;
//    DEBUG_F("Function: decreaseDiameter - New DiameterSP: ");
//    DEBUG_P(diameterSP);
//    notifyClient("diameterSP", String(diameterSP));
//  }
//  if (MessageBuffer == "increaseDiameter") {
//    diameterSP += 0.01;
//    DEBUG_F("Function: increaseDiameter - New DiameterSP: ");
//    DEBUG_P(diameterSP);
//    notifyClient("diameterSP", String(diameterSP));
//  }
//  //Small + - Buttons on Diameter Offset Setpoint
//  if (MessageBuffer == "decreaseDiameterOffset") {
//    diameterOffsetSP -= 0.01;
//    DEBUG_F("Function: decreaseDiameterOffset - New DiameterOffsetSP: ");
//    DEBUG_P(diameterOffsetSP);
//    notifyClient("diameterOffsetSP", String(diameterOffsetSP));
//  }
//  if (MessageBuffer == "increaseDiameterOffset") {
//    diameterOffsetSP += 0.01;
//    DEBUG_F("Function: increaseDiameterOffset - New DiameterOffsetSP: ");
//    DEBUG_P(diameterOffsetSP);
//    notifyClient("diameterOffsetSP", String(diameterOffsetSP));
//  }
//
//  //----------PRESET DROPDOWNS--------------
//
//  if (MessageBuffer == "PLA") {
//    nozzleTempSP = 210;
//    DEBUG_F("Function: Preset - PLA - New NozzleTemp: ");
//    DEBUG_P(nozzleTempSP);
//    notifyClient("nozzleTempSP", String(nozzleTempSP));
//  }
//  if (MessageBuffer == "PET") {
//    nozzleTempSP = 230;
//    DEBUG_F("Function: Preset - PET - New NozzleTemp: ");
//    DEBUG_P(nozzleTempSP);
//    notifyClient("nozzleTempSP", String(nozzleTempSP));
//  }
//  if (MessageBuffer == "ABS") {
//    nozzleTempSP = 250;
//    DEBUG_F("Function: Preset - ABS - New NozzleTemp: ");
//    DEBUG_P(nozzleTempSP);
//    notifyClient("nozzleTempSP", String(nozzleTempSP));
//  }
//
//  if (MessageBuffer == "thin") {
//    diameterSP = 1.75;
//    DEBUG_F("Function: Preset - 1.75 - New Diameter: ");
//    DEBUG_P(diameterSP);
//    notifyClient("diameterSP", String(diameterSP));
//  }
//  if (MessageBuffer == "thick") {
//    diameterSP = 2.85;
//    DEBUG_F("Function: Preset - 2.85 - New Diameter: ");
//    DEBUG_P(diameterSP);
//    notifyClient("diameterSP", String(diameterSP));
//  }
//
//  //----------LEFT NAVIGATION BUTTONS -------
//
//  // Preheat and Cooldown Button
//  if (MessageBuffer == "preheat") {
//    axTruderState = "Heating";
//    notifyClient("axTruderState", axTruderState);
//    preheat();
//  }
//  if (MessageBuffer == "cooldown") {
//    axTruderState = "Cooling";
//    notifyClient("axTruderState", axTruderState);
//    cooldown();
//  }
//  if (MessageBuffer == "forward") {
//    axTruderState = "augerForward";
//    notifyClient("axTruderState", axTruderState);
//    if (augerUnlocked) {
//      rotateForward();
//    }
//  }
//}
//
//  // Reading Nozzle Temperature - driven by JS in Client -> displays in Graph - i am building a JSON String in this crazy section to send Setpoint as well.
//  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
//    String payload = "{\"Temp\":[" + String(readNTC()) + "," + String(nozzleTempSP) + "]}";
//    DEBUG_P("sending message: ");
//    DEBUG_P(payload);
//    request->send(200, "text/plain", payload.c_str());
//    //request->send_P(200, "text/plain", readNTC().c_str());
//  });
//
//  // Reading Diameter - driven by JS in Client -> displays in Graph - i am building a JSON String in this crazy section to send Setpoint as well.
//  server.on("/diameter", HTTP_GET, [](AsyncWebServerRequest * request) {
//    String payload = "{\"Diameter\":[" + String(readDRO()) + "," + String(diameterSP) + "]}";
//    DEBUG_P("sending message: ");
//    DEBUG_P(payload);
//    request->send(200, "text/plain", payload.c_str());
//    //request->send_P(200, "text/plain", readNTC().c_str());
//  });
