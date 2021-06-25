//Auger Motor Stuff

#define AUGER_MOTOR_FORWARD_PIN 27
#define AUGER_MOTOR_REVERSE_PIN 26
#define AUGER_MOTOR_SPEED_PIN 14

#define FREQ 30000
#define PWM_CHANNEL 0
#define RESOLUTION 8

int dutyCycle = 250;

// Auxiliar variables to store the current output state
String augerMotorState = "off";

// ----- Timer -----
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

//----- AUGER SETUP -----
void setupAuger() {
  DEBUG_P("Function: setupAuger started...");

  pinMode(AUGER_MOTOR_FORWARD_PIN, OUTPUT);
  pinMode(AUGER_MOTOR_REVERSE_PIN, OUTPUT);
  pinMode(AUGER_MOTOR_SPEED_PIN, OUTPUT);
  DEBUG_P("Function: setupAuger - Task: pinModes set...");

  // configure LED PWM functionalitites
  ledcSetup(PWM_CHANNEL, FREQ, RESOLUTION);
  DEBUG_P("Function: setupAuger - Task: pwm set...");

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(AUGER_MOTOR_SPEED_PIN, PWM_CHANNEL);
  DEBUG_P("Function: setupAuger - Task: pwm channel attached...");

  // Set outputs to LOW to ensure Stop Motor
  digitalWrite(AUGER_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(AUGER_MOTOR_REVERSE_PIN, LOW);
  DEBUG_P("Function: setupAuger - Task: setting motor directions to low...");

  DEBUG_P("Function: setupAuger - successfull");
}

void rotateForward() { //this function only gets called when you hit the button
  DEBUG_P("Function: rotate Auger Forward started...");
  if (manualOk) { //when the heater is hot but not really stable, we may want to rotate manually and not extrude. -> so let us limit this for a couple of seconds.
    augerOn = true;
    axTruderState = "Manual Forward";
    DEBUG_F("Updating Machine State - New State: ");
    DEBUG_P(axTruderState);
    notifyClient("axTruderState", String(axTruderState));

    for (int i = 0; i < manualRotationTime; i++) {
      augerMotorState = "forward";
      digitalWrite(AUGER_MOTOR_FORWARD_PIN, HIGH);
      digitalWrite(AUGER_MOTOR_REVERSE_PIN, LOW);
      ledcWrite(PWM_CHANNEL, dutyCycle);
      delay(1000); //since we are in a for loop we can't do anything else anyway -> so maybe we should switch to a millis approach here
    }
    augerMotorState = "stopped";
    axTruderState = "Heating"; //so, after stopping the manual mode, we fall back to heating state -> but if nozzle check is ready we go to ready state or hot state
    DEBUG_F("Updating Machine State - New State: ");
    DEBUG_P(axTruderState);
    notifyClient("axTruderState", String(axTruderState));
    augerOn = false;
  }
  else { //if the nozzle is not okay, we do not want to enable the auger
    augerMotorState = "stopped";
    DEBUG_F("");
    axTruderState = "Error";
    DEBUG_F("Updating Machine State - New State: ");
    DEBUG_P(axTruderState);
    notifyClient("axTruderState", String(axTruderState));
  }
}

void rotateReverse() {
  DEBUG_P("Function: rotate Auger Reverse started...");
  if (manualOk) { //when the heater is hot but not really stable, we may want to rotate manually and not extrude. -> so let us limit this for a couple of seconds.
    axTruderState = "Manual Reverse";
    DEBUG_F("Updating Machine State - New State: ");
    DEBUG_P(axTruderState);
    notifyClient("axTruderState", String(axTruderState));

    for (int i = 0; i < manualRotationTime; i++) {
      augerMotorState = "reverse";
      digitalWrite(AUGER_MOTOR_FORWARD_PIN, LOW);
      digitalWrite(AUGER_MOTOR_REVERSE_PIN, HIGH);
      ledcWrite(PWM_CHANNEL, dutyCycle);
      delay(1000); //since we are in a for loop we can't do anything else anyway -> so maybe we should switch to a millis approach here
    }
    augerMotorState = "stopped";
    axTruderState = "Heating"; //so, after stopping the manual mode, we fall back to heating state -> but if nozzle check is ready we go to ready state or hot state
    DEBUG_F("Updating Machine State - New State: ");
    DEBUG_P(axTruderState);
    notifyClient("axTruderState", String(axTruderState));
    augerOn = false;
  }
  else { //if the nozzle is not okay, we do not want to enable the auger
    augerMotorState = "stopped";
    DEBUG_F("");
    axTruderState = "Error";
    DEBUG_F("Updating Machine State - New State: ");
    DEBUG_P(axTruderState);
    notifyClient("axTruderState", String(axTruderState));
  }
}

void stopAuger() {
  DEBUG_P("Function: stopAuger - turning Auger Motor off");
  augerMotorState = "off";
  digitalWrite(AUGER_MOTOR_FORWARD_PIN, LOW);
  digitalWrite(AUGER_MOTOR_REVERSE_PIN, LOW);
  ledcWrite(PWM_CHANNEL, 0);
  augerOn = false;
  extruding = false;
  axTruderState = "Heating";
  DEBUG_F("Updating Machine State - New State: ");
  DEBUG_P(axTruderState);
  notifyClient("axTruderState", String(axTruderState));
}

void extrude(){
  if (nozzleOk) { //now we have the nozzle in a good state to start extruding. when you pressed the forward button, we are extruding basically
    augerMotorState = "forward";
    digitalWrite(AUGER_MOTOR_FORWARD_PIN, HIGH);
    digitalWrite(AUGER_MOTOR_REVERSE_PIN, LOW);
    ledcWrite(PWM_CHANNEL, dutyCycle);
    axTruderState = "Extruding";
    //and we want to tell the interface that we are extruding:
    DEBUG_F("Updating Machine State - New State: ");
    DEBUG_P(axTruderState);
    notifyClient("axTruderState", String(axTruderState));
    augerOn = true;
    extruding = true;
  }
  else { //if the nozzle is not okay, we do not want to enable the auger
    augerMotorState = "stopped";
    DEBUG_F("");
    axTruderState = "Error";
    DEBUG_F("Updating Machine State - New State: ");
    DEBUG_P(axTruderState);
    notifyClient("axTruderState", String(axTruderState));
  }
}
