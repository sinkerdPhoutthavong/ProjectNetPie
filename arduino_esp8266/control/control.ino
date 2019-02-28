#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>

const char* ssid     = "PitiDev";
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
#define relay4 D8

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

  //Relay 1
  if (stateStr == "ON1") {
    digitalWrite(relayPin, LOW);
    microgear.chat("controllerplug", "ON1");
  } else if (stateStr == "OFF1") {
    digitalWrite(relayPin, HIGH);
    microgear.chat("controllerplug", "OFF1");
  }
  //Relay 2
  if (stateStr == "ON2") {
    digitalWrite(relay2, LOW);
    microgear.chat("controllerplug", "ON2");
  } else if (stateStr == "OFF2") {
    digitalWrite(relay2, HIGH);
    microgear.chat("controllerplug", "OFF2");
  }
  //Relay 3
  if (stateStr == "ON3") {
    digitalWrite(relay3, LOW);
    microgear.chat("controllerplug", "ON3");
  } else if (stateStr == "OFF3") {
    digitalWrite(relay3, HIGH);
    microgear.chat("controllerplug", "OFF3");
  }
  //Relay 4
  if (stateStr == "ON4") {
    digitalWrite(relay4, LOW);
    microgear.chat("controllerplug", "ON4");
  } else if (stateStr == "OFF4") {
    digitalWrite(relay4, HIGH);
    microgear.chat("controllerplug", "OFF4");
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
