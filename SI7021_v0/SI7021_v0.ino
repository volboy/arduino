#include <UIPEthernet.h>
#include <SPI.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <MsTimer2.h>
#include <Wire.h>


// SI7021 I2C address is 0x40(64)
#define si7021Addr 0x40
#define TIMEDISPLAY 500 //800 мс период передачи по UART
#define TIMESI7021 1000 // 500 мс период опроса датчика температуры и влажности
#define TIMEPAUSE 150 // 500 мс время паузы между запросами по I2C
#define TIMEMQTT 300

#define AIO_SERVER      "m20.cloudmqtt.com"
#define AIO_SERVERPORT  16260
#define AIO_USERNAME    "kwkfqpfg"
#define AIO_KEY         "3pksm7d6Gp_i"


byte mac[] = {0xAE, 0xB2, 0x26, 0xE4, 0x4A, 0x5C}; // MAC-адрес
byte ip[] = {192, 168, 0, 10};  // IP-адрес клиента
byte myDns[] = {192, 168, 0, 1};  // адрес DNS-сервера
byte gateway[] = {192, 168, 0, 1};  // адрес сетевого шлюза
byte subnet[] = {255, 255, 255, 0};  // маска подсети
EthernetClient client;  // создаем клиента

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish tempPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
//Adafruit_MQTT_Publish humPublish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

//признаки срабатывания таймеров
volatile unsigned int timeDisplay;
volatile unsigned int timeSI7021;
volatile unsigned int timePause;
volatile unsigned int timeMQTT;

float humidity = 0.0;
float temp = 0.0;


void cycleFunc() {
  //таймера
  timeDisplay++;
  timeSI7021++;
  timePause++;
  timeMQTT++;
}
// функция передачи по UART
void funcDisplay() {

  // Output data to serial monitor
  Serial.print(F("Влажность : "));
  Serial.print(humidity);
  Serial.println(F(" % RH"));
  Serial.print(F("Температура : "));
  Serial.print(temp);
  Serial.println(F(" C"));


}
//функция опроса датчика
void funcSI7021() {
  unsigned int data[2]; //массив для данных от датчика
  Wire.beginTransmission(si7021Addr);
  //Send humidity measurement command
  Wire.write(0xF5);
  Wire.endTransmission();
  timePause = 0;
  while (timePause < TIMEPAUSE) {}


  // Request 2 bytes of data
  Wire.requestFrom(si7021Addr, 2);
  // Read 2 bytes of data to get humidity
  if (Wire.available() == 2)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }

  // Convert the data
  humidity  = ((data[0] * 256.0) + data[1]);
  humidity = ((125 * humidity) / 65536.0) - 6;

  Wire.beginTransmission(si7021Addr);
  // Send temperature measurement command
  Wire.write(0xF3);
  Wire.endTransmission();
  timePause = 0;
  while (timePause < TIMEPAUSE) {}

  // Request 2 bytes of data
  Wire.requestFrom(si7021Addr, 2);

  // Read 2 bytes of data for temperature
  if (Wire.available() == 2)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }
  //
  //  // Convert the data
  temp  = ((data[0] * 256.0) + data[1]);
  temp = ((175.72 * temp) / 65536.0) - 46.85;


}


// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void funcMQTT() {

  int8_t ret;


  if (mqtt.connected()) {
    // this is our 'wait for incoming subscription packets' busy subloop
    Adafruit_MQTT_Subscribe *subscription;
    while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &onoffbutton) {
       Serial.print(F("Got: "));
       Serial.println((char *)onoffbutton.lastread);
     }
     }

    // Now we can publish stuff!

    if (! tempPublish.publish(temp)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }

    // ping the server to keep the mqtt connection alive
    if (! mqtt.ping()) {
      mqtt.disconnect();
    }
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(1000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");

}

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  Wire.beginTransmission(si7021Addr);
  Wire.endTransmission();
  delay(500);
  Ethernet.begin(mac, ip);  // инициализация контроллера
  delay(1000);
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  delay(2000);
  mqtt.subscribe(&onoffbutton);
  funcMQTT();
  MsTimer2::set(2, cycleFunc); // 2ms period
  MsTimer2::start();

}

void loop()
{
  if (timeDisplay > TIMEDISPLAY) {
    timeDisplay = 0;
    funcDisplay();
  }
  if (timeSI7021 > TIMESI7021) {
    timeSI7021 = 0;
    funcSI7021();
  }
  if (timeMQTT > TIMEMQTT) {
    timeMQTT = 0;
    funcMQTT();
  }
}
