#include <UIPEthernet.h>



#include <SPI.h>
#include <PubSubClient.h>
#include <MsTimer2.h>
#include <Wire.h>


// SI7021 I2C address is 0x40(64)
#define si7021Addr 0x40
#define TIMEDISPLAY 500 //800 мс период передачи по UART
#define TIMESI7021 1000 // 500 мс период опроса датчика температуры и влажности
#define TIMEPAUSE 150 // 500 мс время паузы между запросами по I2C
#define TIMEMQTT 300

#define AIO_SERVER      "m20.cloudmqtt.com"
#define AIO_SERVERPORT  26260
#define AIO_USERNAME    "kwkfqpfg"
#define AIO_KEY         "3pksm7d6Gp_i"


byte mac[] = {0xAE, 0xB2, 0x26, 0xE4, 0x4A, 0x5C}; // MAC-адрес
byte ip[] = {192, 168, 0, 10};  // IP-адрес клиента
byte myDns[] = {192, 168, 0, 1};  // адрес DNS-сервера
byte gateway[] = {192, 168, 0, 1};  // адрес сетевого шлюза
byte subnet[] = {255, 255, 255, 0};  // маска подсети
EthernetClient client;  // создаем клиента
PubSubClient mqttClient;

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
    char msgBuffer[20];           // убедитесь что стройки размера хватит
   
 
    
    if (mqttClient.connect("ff",AIO_USERNAME,AIO_KEY)) {
    mqttClient.publish("home/temp", dtostrf(temp, 2, 2, msgBuffer));//
    mqttClient.publish("home/humid", dtostrf(humidity, 2, 2, msgBuffer));

   

  }

}

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  Wire.beginTransmission(si7021Addr);
  Wire.endTransmission();
  delay(500);
  Ethernet.begin(mac);  // инициализация контроллера
  mqttClient.setClient(client);
  mqttClient.setServer("m20.cloudmqtt.com", 26260);
  delay(1000);
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  delay(2000);
  
  funcMQTT();
  MsTimer2::set(2, cycleFunc); // 2ms period
  MsTimer2::start();

}

void loop()

{
   mqttClient.loop();
  
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
