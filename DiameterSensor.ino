//esp calipereading based on the work of https://github.com/MGX3D/EspDRO

#include <driver/adc.h>
// ADC threshold for 1.5V SPCsignals (at 6dB/11-bit, high comes to around 1570 in analogRead() )
#define ADC_TRESHOLD 800
// define this to display the raw signal (useful to troubleshoot cable or signal level issues)
#define DEBUG_SIGNAL 0
// timeout in milliseconds for a bit read ($TODO - change to micros() )
#define BIT_READ_TIMEOUT 100
// timeout for a packet read
#define PACKET_READ_TIMEOUT 250
// Packet format: [0 .. 19]=data, 20=sign, [21..22]=unused?, 23=inch
#define PACKET_BITS       24
// minimum reading
#define MIN_RANGE -(1<<20)
// DRO circular buffer entries (4096 entries is roughly the equivalent of 8 minutes of data)
#define DRO_BUFFER_SIZE  0x1000

#define DATA_PIN  36 // yellow
#define CLOCK_PIN 39 // green
//connect ground to ground
//you can drive the Caliper with the battery or use a 1,5V converter (remove battery then)

// capped read: -1 (timeout), 0, 1
int getBit() {
  DEBUG_P("Function: getBit started...");
  int data;

  int readTimeout = millis() + BIT_READ_TIMEOUT;
  while (analogRead(CLOCK_PIN) > ADC_TRESHOLD) {
    if (millis() > readTimeout)
      return -1;
  }

  while (analogRead(CLOCK_PIN) < ADC_TRESHOLD) {
    if (millis() > readTimeout)
      return -1;
  }

  data = (analogRead(DATA_PIN) > ADC_TRESHOLD) ? 1 : 0;
  return data;
}

// read one full packet
long getPacket(){
  DEBUG_P("Function: getPacket started...");
  
  long packet = 0;
  int readTimeout = millis() + PACKET_READ_TIMEOUT;

  int bitIndex = 0;
  while (bitIndex < PACKET_BITS) {
    int bit = getBit();
    if (bit < 0 ) {
      // bit read timeout: reset packet or bail out
      if (millis() > readTimeout) {
        // packet timeout
        return -1;
      }

      bitIndex = 0;
      packet = 0;
      continue;
    }

    packet |= (bit & 1) << bitIndex;

    bitIndex++;
  }

  return packet;
}

// convert a packet to signed microns
long getMicrons(long packet){
  DEBUG_P("Function: getMicrons started...");
  if (packet < 0)
    return MIN_RANGE;

  long data = (packet & 0xFFFFF) * ( (packet & 0x100000) ? -1 : 1);

  if (packet & 0x800000) {
    // inch value (this comes sub-sampled)
    data = data * 254 / 200;
  }

  return data;
}

void setupDRO() {
  DEBUG_P("Function: setupDRO started...");
  
  pinMode(DATA_PIN, INPUT);
  pinMode(CLOCK_PIN, INPUT);

  delay(20);
  analogReadResolution(11);

  analogSetAttenuation(ADC_6db);
  adc1_config_width(ADC_WIDTH_BIT_10);
}

String readDRO() {
  DEBUG_P("Function: readDRO started...");
  String diameter = String(getMicrons(getPacket()));
  // put your main code here, to run repeatedly:
  DEBUG_P("Messung: ");
  DEBUG_P(diameter);
  return diameter;
}
