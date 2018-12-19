#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

const char* ssid     = "pitidev";
const char* password = "ssssssss";

#define APPID       "LaoSmartfarm"
#define GEARKEY     "4F8dSA9hfL0cjYb"
#define GEARSECRET  "OPH3LsTf5Wi5T52wEll5kbB7d"
#define SCOPE       "PitiControl"
#define ALIAS   "PitiControl"

WiFiClient client;
AuthClient *authclient;

//Relay Control
#define led0 D0  //set status connect wifi
#define relayPin D1
#define relay2 D2
#define relay3 D3

MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  Serial.print(topic);
  Serial.print(" : ");
  char strState[msglen];
  for (int i = 0; i < msglen; i++) {
    strState[i] = (char)msg[i];
    Serial.print((char)msg[i]);
  }
  Serial.println();

  String stateStr = String(strState).substring(0, msglen);

  //Relay One
  if (stateStr == "ON") {
    digitalWrite(relayPin, LOW);
    microgear.chat("controllerplug", "ON");
  } else if (stateStr == "OFF") {
    digitalWrite(relayPin, HIGH);
    microgear.chat("controllerplug", "OFF");
  }
  //Relay Two
  if (stateStr == "ON1") {
    digitalWrite(relay2, LOW);
    microgear.chat("controllerplug", "ON1");
  } else if (stateStr == "OFF1") {
    digitalWrite(relay2, HIGH);
    microgear.chat("controllerplug", "OFF1");
  }
  //Relay Three
  if (stateStr == "ON2") {
    digitalWrite(relay3, LOW);
    microgear.chat("controllerplug", "ON2");
  } else if (stateStr == "OFF2") {
    digitalWrite(relay3, HIGH);
    microgear.chat("controllerplug", "OFF2");
  }
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setName("pieplug");
}

void setup() {
  Serial.begin(115200);

  Serial.println("Starting...");

  pinMode(relayPin, OUTPUT);
  pinMode(led0, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);

  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);

  if (WiFi.begin(ssid, password)) {

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    //uncomment the line below if you want to reset token -->
    microgear.resetToken();
    microgear.init(GEARKEY, GEARSECRET, SCOPE);
    microgear.connect(APPID);
  }
}

void loop() {
  if (microgear.connected()) {
    microgear.loop();
    Serial.println("connect...");
    digitalWrite(led0, HIGH);
  } else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }
  delay(1000);
}
