#define NUMSAMPLES 10
#define THERMISTOR_PIN 34

double adcMax, Vs;

double R1 = 10000.0;   // voltage divider resistor value
double Beta = 3950.0;  // Beta value
double To = 298.15;    // Temperature in Kelvin for 25 degree Celsius
double Ro = 10000.0;   // Resistance of Thermistor at 25 degree Celsius

int samples[NUMSAMPLES];

void setupNTC(){
  DEBUG_P("Function: setupNTC started...");
    //  analogReference(EXTERNAL);

  adcMax = 4095.0; // ADC resolution 12-bit (0-4095)
  Vs = 3.3;        // supply voltage
  pinMode(THERMISTOR_PIN, INPUT);
}

String readNTC(){
  DEBUG_P("Function: readNTC started...");

  //Working Vars in local Space
  float Vout, Rt = 0;
  float T, Tc, averageTc, Tf = 0;
  uint8_t i;
  float average;

  //Measure a few times to calculate average
  for (i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(THERMISTOR_PIN);
    delay(10);
  }
  
  //calculate average
  average = 0;
  for (i = 0; i < NUMSAMPLES; i++) {
    average += samples[i];
  }
  average /= NUMSAMPLES;
  DEBUG_F("Average analog reading: ");
  DEBUG_P(String(average));

  average = adcMax / average;
  average = R1 / average;
  DEBUG_F("Thermistor resistance: ");
  DEBUG_P(String(average));
  
  //This is the Math to Calculate the correct Temperature
  float steinhart;
  steinhart = average / Ro;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= Beta;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (To); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert absolute temp to C

  DEBUG_F("Calculated Temperature: ");
  DEBUG_P(String(steinhart));
  DEBUG_P(" *C");
  //delay(2000);
  
  return String(steinhart);
}
