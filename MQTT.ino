//MQTT

#include <PubSubClient.h>

const char* mqttServer = "192.168.0.100";
const char* mqttUsername = "openhabian";
const char* mqttPassword = "Sc13nc3B1tch";
const char* mqttDeviceId = "axTruder";

const char* temperatureTopic = "stat/nozzleTemperature";
const char* temperatureSPTopic = "stat/nozzleTemperatureSP" ;
const char* diameterTopic = "stat/diameter";
const char* diameterSPTopic = "stat/diameterSP";
WiFiClient axTruder;
PubSubClient client(axTruder);

void setupMQTT(){
  DEBUG_P("Function: setupMQT started...");
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

//----------------------------------- RECONNECT MQTT ---------------------
void reconnect() {
  DEBUG_P("Function: MQTT reconnect started...");
  // Loop until we're reconnected
  DEBUG_P("reconnect: ");
  while (!client.connected()) {
    DEBUG_P("Attempting MQTT connection...");

    // Attempt to connect
    if (client.connect(mqttDeviceId, mqttUsername, mqttPassword)) {
      DEBUG_P("connected to MQTT-Broker");

      client.subscribe("cmnd/axTruderCommands"); //Feel free to Add more Topics here
    } else {
      DEBUG_P("failed, rc=");
      DEBUG_P(String(client.state()));
      DEBUG_P(" try again in 5 seconds");

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//----------------------------------- CALLBACK MQTT ---------------------
void callback(char* topic, byte* message, unsigned int length) {
  DEBUG_P("Function: MQTT_callback started...");
  DEBUG_P("callback: ");
  DEBUG_P("Message arrived on topic: ");
  DEBUG_P(String(topic));
  DEBUG_P(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    DEBUG_P((char)message[i]);
    messageTemp += (char)message[i];
  }
  
  //Here you can add some commands for remote control
  if (String(topic) == "cmnd/axTruderCommands") {
    DEBUG_P("Changing Mode to");
    if (messageTemp == "stop") {
      DEBUG_P("STOP");
      //do stuff
    }
    else if (messageTemp == "start") {
      DEBUG_P("START");
      //do other stuff
    }
  }
}
//----------------------------------- Publish MQTT ---------------------
void publishMQTT() {
  DEBUG_P("publishMQTT: Sende Nachrichten...");
  unsigned long now = millis();
  
    client.publish(temperatureTopic, String(readNTC()).c_str());
    client.publish(temperatureSPTopic, String(nozzleTempSP).c_str());
    client.publish(diameterTopic, String(readDRO()).c_str());
    client.publish(diameterSPTopic, String(diameterSP).c_str());
 
}
