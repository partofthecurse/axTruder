/*
   Here is all the Communication between the UI in your Browser (the Client) and the ESP Programm Logic.
   Basically we are chatting in JSON Strings and if a special action, task and value are received, we check for them and the logic knows what to do.
   This way we can update setpoints and control the program logic
*/

String MessageBuffer = "no Message received"; //here we store the received message


//here we handle all the commands
void handleWebSocketMessage() {

  DEBUG_P("Function: handleWebSocketMessage started");
  //Check if we send a direct message or if we use JSON (need to fix that but to lazy atm, so use this workaround)
  //We use the following JSON String Format to communicat: {"action":"nozzleTemp","task":"setValue","value":"210"} to receive commands

  DeserializationError error = deserializeJson(jsonBuffer, MessageBuffer); //we deserialise the json string and check for erros.

  // Test if parsing succeeds.
  if (error) {
    DEBUG_P("deserializeJson() failed: ");
    DEBUG_P(error.f_str());
    return;
  }

  else { //when there is no error, we can check for the action
    String action = jsonBuffer["action"];
    String task = jsonBuffer["task"];
    DEBUG_F("Received action: ");
    DEBUG_P(action);
    DEBUG_F("Received task: ");
    DEBUG_P(task);

    //Check which action and then check for corresponding tasks
    if (action == "nozzleTemp") {

      //If we get a message from setTemp() js function:
      if (task == "setValue") {
        nozzleTempSP = jsonBuffer["value"];
        DEBUG_F("Function: setTemp");
      }

      //if we get message from increaseTemp() JS
      if (task == "increase") {
        nozzleTempSP += 1;
        DEBUG_F("Function: increaseTemp");
      }

      //if we get message from decreaseTemp() JS
      if (task == "decrease") {
        nozzleTempSP -= 1;
        DEBUG_F("Function: decreaseTemp");
      }
      //Presets from Dropdown
      if (task == "PLA") {
        nozzleTempSP = 210;
        DEBUG_F("Function: Preset - PLA");
      }
      if (task == "PET") {
        nozzleTempSP = 230;
        DEBUG_F("Function: Preset - PET");
      }
      if (task == "ABS") {
        nozzleTempSP = 250;
        DEBUG_F("Function: Preset - ABS");
      }
      DEBUG_F(" - New Setpoint Value: ");
      DEBUG_P(nozzleTempSP);
      notifyClient("nozzleTempSP", String(nozzleTempSP));;

      //if we get message from setTempOffset() JS
      if (task == "setOffset") {
        nozzleTempOffsetSP = jsonBuffer["value"];
        DEBUG_F("Function: setTempOffset");
      }

      //if we get message from increaseTempOffset() JS
      if (task == "increaseOffset") {
        nozzleTempOffsetSP += 1;
        DEBUG_F("Function: increaseTempOffset");
      }

      //if we get message from decreaseTempOffset() JS
      if (task == "decreaseOffset") {
        nozzleTempOffsetSP -= 1;
        DEBUG_F("Function: decreaseTempOffset");
      }
      DEBUG_F(" - New Offset Value: ");
      DEBUG_P(nozzleTempOffsetSP);
      notifyClient("nozzleTempOffsetSP", String(nozzleTempOffsetSP));
    } //End of action nozzleTemp

    //if the action is diameter
    if (action == "diameter") {

      //If we get a message from setDiameter() js function:
      if (task == "setValue") {
        diameterSP = jsonBuffer["value"];
        DEBUG_F("Function: setDiameter");
      }

      //if we get message from increaseDiameter() JS
      if (task == "increase") {
        diameterSP += 0.01;
        DEBUG_F("Function: increaseDiameter");
      }

      //if we get message from decreaseDiameter() JS
      if (task == "decrease") {
        diameterSP -= 0.01;
        DEBUG_F("Function: decreaseDiameter");
      }

      //Presets from Dropdown
      if (task == "thin") {
        diameterSP = 1.75;
        DEBUG_F("Function: Preset - 1.75");
      }
      if (task == "thick") {
        diameterSP = 2.85;
        DEBUG_F("Function: Preset - 2.85");
      }
      DEBUG_F(" - New Diameter Value: ");
      DEBUG_P(diameterSP);
      notifyClient("diameterSP", String(diameterSP));

      //if we get message from setDiameterOffset() JS
      if (task == "setOffset") {
        diameterOffsetSP = jsonBuffer["value"];
        DEBUG_F("Function: setdiameterOffset");
      }
      //if we get message from increaseDiameterOffset() JS
      if (task == "increaseOffset") {
        diameterOffsetSP += 0.01;
        DEBUG_F("Function: increaseDiameterOffset");
      }

      //if we get message from decreaseDiameterOffset() JS
      if (task == "decreaseOffset") {
        diameterOffsetSP -= 0.01;
        DEBUG_F("Function: decreaseDiameterOffset");
      }

      DEBUG_F(" - New Offset Value: ");
      DEBUG_P(diameterOffsetSP);
      notifyClient("diameterOffsetSP", String(diameterOffsetSP));
    }//--- End of action diameter

    if (action == "auger") { // here starts the program logic - > user is setting the presets and then starts the journey with a push to the preheat button
      if (task == "preheat") {
        DEBUG_F("Function: preheat");
        axTruderState = "Heating";
        preheat(); // we continue at the preheat function in tab HeaterControl
      }

      if (task == "cooldown") {
        DEBUG_F("Function: cooldown");
        axTruderState = "Cooling";
        cooldown();
      }

      if (task == "forward") {
        DEBUG_F("Function: Rotation Forward");
        axTruderState = "Extruding";
        rotateForward();
        //maybe add further state logic here (if temp == setpoint +- tolerance - extruding)
      }

      if (task == "reverse") {
        DEBUG_F("Function: Rotation Backwards");
        axTruderState = "Reversing";
        rotateReverse();
      }

      if (task == "stop") {
        DEBUG_F("Function: Rotation Stop");
        axTruderState = "AUGER STOP";
        stopAuger();
        //maybe more logic here as well (check temp to process state)
      }

      if (task == "setSpeed") {
        DEBUG_F("Function: set Auger Speed");
        dutyCycle = jsonBuffer["value"];
        DEBUG_F(" - New Speed: ");
        DEBUG_P(dutyCycle);
        notifyClient("augerSpeedSP", String(dutyCycle));
      }

      if (task == "setTimeFrame") {
        DEBUG_F("Function: set Auger Manual Control Period");
        manualRotationTime = jsonBuffer["value"]:
        DEBUG_F(" - New Period: ");
        DEBUG_P(manualRotationTime);
        //notifyClient("augerSpeedSP", String(dutyCycle));
      }

      DEBUG_F(" - New State: ");
      DEBUG_P(axTruderState);
      notifyClient("axTruderState", String(axTruderState));
    }//--- End of action "auger"

    if (action == "fan") {
      if (task == "start") {
        DEBUG_F("Function: startFan");
        fanOn();
      }
      if (task == "stop") {
        DEBUG_F("Function: stopFan");
        fanOff();
        //maybe more logic here as well (check temp to process state)
      }
      if (task == "setpeed") {
        DEBUG_F("Function: set Fan Speed");
        dutyCycleFan = jsonBuffer["value"];
        //maybe more logic here as well (check temp to process state)
      }
      DEBUG_F(" - New Value: ");
      DEBUG_P(dutyCycleFan);
      notifyClient("fanSpeedSP", String(dutyCycleFan));
    }
  }
}

//Sending Answers and Updates as JSON String back to the Client. Gets called if we received a command or when the state in programm changed.
//The Receiving JSON is more complex, but the answer is alway just a key and value pair.
void notifyClient(String wsKey, String wsValue) {
  String payload = "{\"" + String(wsKey) + "\":\"" + String(wsValue) + "\"}"; //this is just a rookie way to form a json string.
  ws.textAll(String(payload));
}

//Main Websocket Events
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  DEBUG_P("Function: onEvent (Websockets)");
  switch (type) {
    case WS_EVT_CONNECT:
      DEBUG_F("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      DEBUG_F("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      DEBUG_P("Websocket received Message: ");
      MessageBuffer = "";
      for (int i = 0; i < len; i++) {
        Serial.print((char) data[i]);
        MessageBuffer = MessageBuffer + (char) data[i];
      }
      DEBUG_F(" - Message End. Stored to Buffer: ");
      DEBUG_P(MessageBuffer);
      handleWebSocketMessage();
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

//start Websocket
void initWebSocket() {
  DEBUG_P("Function: init Websocket started...");
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
