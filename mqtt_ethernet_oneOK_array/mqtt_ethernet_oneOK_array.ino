#include <UIPEthernet.h>
#include <utility/logging.h>

/***************************************************
  Adafruit MQTT Library Ethernet Example

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Alec Moore
  Derived from the code written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <SPI.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long timeMQTT = 10000;           // interval at which to blink (milliseconds)
const long timeWater=100;
/************************* Ethernet Client Setup *****************************/
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

typedef union{
    //Customize struct with whatever variables/types you like.

    struct __attribute__((__packed__)){  // packed to eliminate padding for easier parsing.
        char myChar[12];
        
    }s;

    uint8_t raw[sizeof(s)];                    // For publishing

} packet_t;

packet_t myPacket;
//Uncomment the following, and set to a valid ip if you don't have dhcp available.

//Uncomment the following, and set to your preference if you don't have automatic dns.
//IPAddress dnsIP (8, 8, 8, 8);
//If you uncommented either of the above lines, make sure to change "Ethernet.begin(mac)" to "Ethernet.begin(mac, iotIP)" or "Ethernet.begin(mac, iotIP, dnsIP)"

byte ip[] = {192, 168, 137, 104};  // IP-адрес
//byte myDns[] = {192, 168, 137, 1};  // адрес DNS-сервера
//byte gateway[] = {192, 168, 137, 1};  // адрес сетевого шлюза
//byte subnet[] = {255, 255, 255, 0};  // маска подсети

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "m15.cloudmqtt.com"
#define AIO_SERVERPORT  17518
#define AIO_USERNAME    "thhbkpya"
#define AIO_KEY         "T43U2z-CEsrB"


/************ Global State (you don't need to change this!) ******************/

//Set up the ethernet client
EthernetClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// You don't need to change anything below this line!
#define halt(s) { Serial.println(F( s )); while(1);  }


/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish waterDog = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/waterDog");

// Setup a feed called 'onoff' for subscribing to changes.
//Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

/*************************** Sketch Code ************************************/

void setup() {

  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  Serial.begin(115200);

  Serial.println(F("Adafruit MQTT demo"));

  // Initialise the Client
  Serial.print(F("\nInit the Client..."));
   Ethernet.begin(mac, ip);
  
  Serial.println(Ethernet.localIP());
//  Serial.println(Ethernet.subnetMask());
//  Serial.println(Ethernet.gatewayIP());
//  Serial.println(Ethernet.dnsServerIP());

  delay(1000); //give the ethernet a second to initialize
  

  //mqtt.subscribe(&onoffbutton);
}



void loop() {
    unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= timeMQTT) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
      // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
//  Adafruit_MQTT_Subscribe *subscription;
//  while ((subscription = mqtt.readSubscription(1000))) {
//    if (subscription == &onoffbutton) {
//      Serial.print(F("Got: "));
//      Serial.println((char *)onoffbutton.lastread);
//    }
//  }

 
 
  if (! waterDog.publish(myPacket.raw, sizeof(packet_t))) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  // ping the server to keep the mqtt connection alive
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
    
    }
  

  if (digitalRead(2)==true) {myPacket.s.myChar[0]=1;}
  if (digitalRead(3)==true) {myPacket.s.myChar[1]=1;}
  if (digitalRead(4)==true) {myPacket.s.myChar[2]=1;}
  if (digitalRead(5)==true) {myPacket.s.myChar[3]=1;}
  if (digitalRead(6)==true) {myPacket.s.myChar[4]=1;}
  if (digitalRead(7)==true) {myPacket.s.myChar[5]=1;}
  if (digitalRead(A0)==true) {myPacket.s.myChar[6]=1;}
  if (digitalRead(A1)==true) {myPacket.s.myChar[7]=1;}
  if (digitalRead(A2)==true) {myPacket.s.myChar[8]=1;}
  if (digitalRead(A3)==true) {myPacket.s.myChar[9]=1;}
  if (digitalRead(A4)==true) {myPacket.s.myChar[10]=1;}
  if (digitalRead(A5)==true) {myPacket.s.myChar[11]=1;}
 

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 2 seconds...");
       mqtt.disconnect();
       delay(2000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}
