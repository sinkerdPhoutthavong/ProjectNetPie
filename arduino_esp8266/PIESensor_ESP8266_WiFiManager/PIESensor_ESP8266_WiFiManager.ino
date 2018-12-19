/*  NETPIE ESP8266 basic sample                            */
/*  More information visit : https://netpie.io             */

#include <ESP8266WiFi.h>
#include <MicroGear.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include "DHT.h"

#define APPID   "LaoSmartfarm"       // e.g. "MySwitch"
#define KEY     "4F8dSA9hfL0cjYb"      // e.g. "4DPanXKaSdb2VrT"
#define SECRET  "OPH3LsTf5Wi5T52wEll5kbB7d"   // e.g. "ZgrXbHsaVp7TI8xW5oEcAqvY3"
#define ALIAS   "dht"

#define DHTPIN D1
#define DHTTYPE DHT11
#define REDPIN D7
#define GREENPIN D6
#define BLUEPIN D5
#define LEDPIN D3

WiFiClient client;

DHT dht(DHTPIN, DHTTYPE);
int LDR = A0;
int humid = 0;
int temp = 0;
int light = 0;
int counter = 0;
String msgstr;

int timer = 0;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  Serial.println((char *)msg);
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i=0; i<msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();  
  microgear.publish("/piesensor",msgstr);
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i=0; i<msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
}

void connectWifi(){
    WiFiManager wifiManager;
    wifiManager.setTimeout(60);
    digitalWrite(BLUEPIN, HIGH);
    if(!wifiManager.autoConnect("PIESensor")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      ESP.reset();
      delay(5000);
    }
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());  
    digitalWrite(BLUEPIN, LOW);
}

void setup() {
    /* Event listener */
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(PRESENT,onFoundgear);
    microgear.on(ABSENT,onLostgear);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    Serial.println("Starting...");

    dht.begin();
    pinMode(REDPIN, OUTPUT);
    pinMode(GREENPIN, OUTPUT);
    pinMode(BLUEPIN, OUTPUT);
    pinMode(LEDPIN, OUTPUT);
    digitalWrite(REDPIN, LOW);
    digitalWrite(GREENPIN, LOW);
    digitalWrite(BLUEPIN, LOW);
    digitalWrite(LEDPIN, LOW);
    
    Serial.println();
    Serial.println("Wait for WiFi... ");
    
    connectWifi();
    
    /* Initial with KEY, SECRET and also set the ALIAS here */
    microgear.init(KEY,SECRET,ALIAS);
    microgear.connect(APPID);
}

void loop() {
  if(WiFi.status() != WL_CONNECTED){
    connectWifi();
    microgear.connect(APPID);
  }else{
    /* To check if the microgear is still connected */
    if (microgear.connected()) {
//      Serial.println("connected");
      digitalWrite(GREENPIN, HIGH);
      /* Call this method regularly otherwise the connection may be lost */
      microgear.loop();
      
//      if (timer >= 1000) {
//        Serial.println("Publish...");
        float LDRValue = (float)analogRead(LDR); 
        if(LDRValue<0){
          LDRValue = 0.0;
        }else if(LDRValue>600){
          LDRValue = 600.0;
        }
        LDRValue = LDRValue/60.0;
        //Serial.println(LDRValue);
        if(LDRValue<=3){
          digitalWrite(LEDPIN, HIGH);
        }else{
          digitalWrite(LEDPIN, LOW);
        }
        
        float vhud = dht.readHumidity();
        float vtmp = dht.readTemperature();
        if (isnan(vhud) || isnan(vtmp) || vhud > 100 || vtmp > 100){
          vhud = 0.0;
          vtmp = 0.0;
        }
        
        msgstr = (String)vtmp+","+(String)vhud+","+LDRValue+",esp8266";
        Serial.println(msgstr);
        Serial.println(humid!=(int)vhud || temp!=(int)vtmp || light!=(int)LDRValue);

        if(humid!=(int)vhud || temp!=(int)vtmp || light!=(int)LDRValue){
          microgear.publish("/piesensor",msgstr);
          humid=(int)vhud;
          temp=(int)vtmp;
          light=(int)LDRValue;
          counter=0;  
        }else{
          if(counter==5){
            microgear.publish("/piesensor",msgstr);
            counter=0;  
          } 
          counter++;
        }
        timer = 0;
//      }
    }
    else {
      digitalWrite(GREENPIN, LOW);
      if (timer >= 5000) {
        Serial.println("connection lost, reconnect...");
        microgear.connect(APPID);
        timer = 0;
      }
      else timer += 500;
    }
    delay(500);
  }
}
