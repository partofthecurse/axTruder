//NOZZLE FAN CONTROL

#define FAN_SPEED_PIN 8

#define FREQ 30000
#define PWM_CHANNEL_FAN 1
#define RESOLUTION 8

int dutyCycleFan = 250;

// Auxiliar variables to store the current output state
bool fanState = false; 

//// ----- Timer -----
//// Current time
//unsigned long currentFanTime = millis();
//// Previous time
//unsigned long previousFanTime = 0;
//// Define timeout time in milliseconds (example: 2000ms = 2s)
//const long timeoutFanTime = 2000;

//----- FAN SETUP -----
void setupFan() {
  DEBUG_P("Function: setupFan started...");

  pinMode(FAN_SPEED_PIN, OUTPUT);
  DEBUG_P("Function: setupFan - Task: pinModes set...");

  // configure LED PWM functionalitites
  ledcSetup(PWM_CHANNEL_FAN, FREQ, RESOLUTION);
  DEBUG_P("Function: setupFan - Task: pwm set...");

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(FAN_SPEED_PIN, PWM_CHANNEL_FAN);
  DEBUG_P("Function: setupFan - Task: pwm channel attached...");

  DEBUG_P("Function: setupFan - successfull");
}

void fanOn() {
  DEBUG_P("Function: fanOn started...");
  fanState = true;
  ledcWrite(PWM_CHANNEL_FAN, dutyCycleFan);  
  notifyClient("fanOn", String(dutyCycleFan));  
}

void fanOff() {
  DEBUG_P("Function: fanOff started...");
  fanState = false;
  ledcWrite(PWM_CHANNEL_FAN, dutyCycleFan);
  notifyClient("fanOff", String(dutyCycleFan));
}
