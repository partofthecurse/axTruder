/*We control Diameter with PID (Speed difference Spool to Nozzle)
  But we do not measure the actual speed. i use a lookup table instead
  Since we are driving the Auger and Spoolwinder from same Motor driver i will use the dutyCycle of Auger as reference to dertermine the beginning speed.
  we know the gear of the motor so on max dutycycle (255) we know the revolutions per minute (in my case, it is 32). we will then calculate the dutycycle needed for exact same r/m for spoolwinder (we know the motor)
  
  maybe using stepper motors is more elegant.
*/


//Include the PID LIB
#include <PID_v1.h>

#define SPOOL_SPEED_PIN 9 //Pin for the Spool
#define SPOOL_FORWARD_PIN 10
#define SPOOL_REVERSE_PIN 11

#define FREQ_D 30000
#define PWM_CHANNEL_D 2
#define RESOLUTION_D 8

int dutyCycleD = 250;

float diameterToleranceMax = 0.05; //This is the upper diameter tolerance
float diameterToleranceMin = 0.05; //This is the minimum diameter tolerance

//Define Vars for the PID controller
double SetpointD, InputD, OutputD;

double DKp = 2.0;
double DKi = 0.2;
double DKd = 0.0;


PID diameterPID(&InputD, &OutputD, &SetpointD, DKp, DKi, DKd, DIRECT); //PID Controller constructor

//--- Function to connect Pins and stuff
void setupSpool() {
  DEBUG_P("Function: setupSpool...");
  //Defining the Outputs
  pinMode(SPOOL_SPEED_PIN, OUTPUT);
  pinMode(SPOOL_FORWARD_PIN, OUTPUT);
  pinMode(SPOOL_REVERSE_PIN, OUTPUT);

  //initialise the PID
  SetpointD = diameterSP + diameterOffsetSP; //calculate the real setpoint from UI
  DEBUG_P("Calculated Setpoint...");
  diameterPID.SetOutputLimits(100, 255); //since our motorcontroller has range from 0 to 255, but below 100 it basically stops already, we set min max output to 100/255
  //turn the PID on
  diameterPID.SetMode(AUTOMATIC);
  DEBUG_P("Initialised PID...");

  // configure LED PWM functionalitites (used for speed control of Spool winder)
  ledcSetup(PWM_CHANNEL_D, FREQ_D, RESOLUTION_D);
  DEBUG_P("Function: setupSpool - Task: pwm set...");

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(SPOOL_SPEED_PIN, PWM_CHANNEL_D);
  DEBUG_P("Function: setupSpool - Task: pwm channel attached...");

  // Set outputs to LOW to ensure Stop Motor
  digitalWrite(SPOOL_FORWARD_PIN, LOW);
  digitalWrite(SPOOL_REVERSE_PIN, LOW);
  DEBUG_P("Function: setupSpool - Task: setting motor directions to low...");

  DEBUG_P("Function: setupSpool - successfull");
}

void controlDiameter() {
  DEBUG_P("Function: controlDiameter");  
  
  if (millis() % 1000 == 0) { //We do this part every second to get the current Diameter
    unsigned long zeit = (millis() / 1000);
    InputD = readDRO().toInt(); // We read the DRO get the current diameter
    DEBUG_P(String(zeit));
    DEBUG_F("; ");
    DEBUG_P(String(InputD));
    DEBUG_F("; ");
    DEBUG_P(String(OutputD));
    DEBUG_F(" ms");
  }
  InputD = readDRO().toInt(); //uncomment if you need every loop sensor reading, comment if not
  diameterPID.Compute(); //tells the lib to do its job
  controlSpool(OutputD); //start control spool with the output value (already mapped here between 0 and 255
}

// Function to Get the PID Output to the Spoolwinder
void controlSpool(double SP){
  if (nozzleOk && augerOn){
    dutyCycleD = OutputD;
  }
  
}
