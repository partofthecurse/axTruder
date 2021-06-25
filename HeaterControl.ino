/*
   Temperature COntrol
   -Using the PID LIbrary
   - Every call on heatNozzle is reading the thermistor to give the current temperature value to the pid lib. then the output is calculated
   - for the heater we use a heating window of x seconds (i use 5) to calculate a heating time and a low time -> to have some kind of slow pwm if you will
*/

//Include the PID LIB
#include <PID_v1.h>

//Define some Pins
#define HEATER_PIN 23

//this is the tolerance in between we want to be to give the heater okay signal and to unlock auger control
int tempToleranceMax = 5;
int tempToleranceMin = 5;

float lastErrorTime;
float errorTimeFrame = 30000; // for checking if we are in tolerance for atleast 30 seconds

//Define Vars for the PID controller
double Setpoint, Input, Output;

double Kp = 2.0;
double Ki = 0.2;
double Kd = 0.0;

unsigned long WindowSize = 5000; //this is our Control-Window in MS

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT); //PID Controller constructor

//--- Function to connect Pins and stuff
void setupHeater() {
  DEBUG_P("Function: setupHeater...");
  //Defining the Outputs
  pinMode(HEATER_PIN, OUTPUT);
  digitalWrite(HEATER_PIN, LOW);

  //initialise the PID
  Setpoint = nozzleTempSP + nozzleTempOffsetSP; //calculate the real setpoint from UI
  DEBUG_P("Calculated Setpoint...");
  myPID.SetOutputLimits(0, WindowSize);         //this makes the pid lib recalculate the output value to our timeframe -> basically a chart mapping
  //turn the PID on
  myPID.SetMode(AUTOMATIC);                     //there are some other mode but no clue of that.
  DEBUG_P("Initialised PID...");
}

//starts the preheat procedure
void preheat() {                          //after the user startet this we fire up the heater
  DEBUG_P("Function: preheat");
  stopAuger(); //if not already
  heaterOn = true; //if this is true, in the main arduino loop the function heatNozzle will be called every loop -> this is the next step in program logic
}

void cooldown() {
  DEBUG_P("Function: cooldown");
  stopAuger();        //if we want to cool down the nozzle, we should make sure the auger does not move
  heaterOn = false; //we just set this to false to exclude the heatNozzle from the loop again
  digitalWrite(HEATER_PIN, LOW);  //and this is just a safety call to make sure heater is low
  augerOn = false;                //so, we need to lock the auger controls again
}

void heatNozzle() { //if heaterOn Boolean is true, this function is called in main loop on every loop -> this controls the nozzle temperature
  DEBUG_P("Function: heatNozzle");

  if (millis() % 1000 == 0) { //We do this part every second to get the current Temperature -> mostly for debugging
    unsigned long zeit = (millis() / 1000);
    Input = readNTC().toInt();
    DEBUG_P(String(zeit));
    DEBUG_F("; ");
    DEBUG_P(String(Input));
    DEBUG_F("; ");
    DEBUG_P(String(Output));
    DEBUG_F(" ms");
  }
  // Input = readNTC(); //uncomment if you need every loop sensor reading
  myPID.Compute(); //now we let the PID LIB do its magic
  sps(Output);    //and call the controller routine to actually turn on off the heater in a time window manner
}

/************************************************
   turn the output pin on/off based on pid output
 ************************************************/

void sps(double hz) {
  //https://de.wikipedia.org/wiki/Schwingungspaketsteuerung

  static unsigned long szl;
  static unsigned long szh;               //the first low time is the window time minus the output from the pid
  unsigned long lzl = WindowSize - hz;
  unsigned long lzh = hz;

  if (!digitalRead(HEATER_PIN) && millis() - szl >= lzl ) { //if heaterpin is low and the current time - start time low is bigger than last time low
    szh = millis();                                         //start time high is current time
    digitalWrite(HEATER_PIN, HIGH);                         //heater on!
  }
  if (digitalRead(HEATER_PIN) && millis() - szh >= lzh ) { // if heaterpin is on and the current time minus start time high is bigger than last time low
    szl = millis();
    digitalWrite(HEATER_PIN, LOW);                        //heater on
  }
}
//nice, now we have our temperature controlled by the PID which is controlling our SSR to turn on / off the heater.
//next, when we are close enough to setpoint and did not exit the tolerance for a decent amount of time, we can set a bool to a okay state

void checkNozzle() { //we also only need to call this when the heater is actually on -> so this function is also called in main loop if heaterON is true...
  if (Input > Setpoint - tempToleranceMin){
    manualOk = true;
    axTruderState = "Hot";
    DEBUG_F("Updating Machine State - New State: ");
    DEBUG_P(axTruderState);
    notifyClient("axTruderState", String(axTruderState));
  }
  if (Input > tempToleranceMax + Setpoint || Input < Setpoint - tempToleranceMin) { //so, if this is the case, we want to reset a timer
    lastErrorTime = millis();
  }
  if (millis() - lastErrorTime >= errorTimeFrame) { //so if now minus the last error time is bigger than a given timeframe, we know that there was no recent error.
    if (!nozzleOk){ //now we could set the nozzle OKay, but if we come from not okay, we want to update the ui and unlock the buttons -> but we do not want to send this message all the time so this if takes care of that
      axTruderState = "Ready";
      DEBUG_F("Nozzle seems to be okay - New State: ");
      DEBUG_P(axTruderState);
      notifyClient("axTruderState", String(axTruderState)); //-> with this message the ui will unlock the buttons
    }
    nozzleOk = true; //now we set it okay, therefore the above code will not be called, only if we go to notOK (nozzleOk == false) again.
  }
}
//great, when we are in tolerance, we can start extrude -> have a look at the auger control now (Tab: Auger)


//if diameter outside tolerance -> fix with speed difference -> alarm
//detect revolutions of spool somehow -> calculate length? -> auto stop when full
//maybe use weight sensor instead
