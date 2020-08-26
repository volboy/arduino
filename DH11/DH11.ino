#include <iarduino_OLED_txt.h>
#include <DHT.h>
#include <DHT_U.h>



#define DHTPIN 2 //сигнальный пин с датчика влажности
DHT dht(DHTPIN, DHT11); //инициализация датчика
iarduino_OLED_txt myOLED(0x78); //инициализация дисплея с адресом по шине I2C

extern uint8_t MediumFontRus[]; //подключаем шрифт

void setup() {
  Serial.begin(9600);
  dht.begin();
  myOLED.begin();
  myOLED.setFont(MediumFontRus);
    }

void loop() {
  delay(2000);
  float h=dht.readHumidity(); 
  float t=dht.readTemperature();
  if (isnan(h)||isnan(t)) { //проверка, если не удвется считать данные то вывести ошибка считывания
    Serial.print("Ошибка считывания");
    myOLED.print("Ошибка считывания", 0,0);
    return;
      }
      myOLED.print("h=", 0,1);
      myOLED.print(h);
      myOLED.print(" %");
      myOLED.print("t=", 0,3);
      myOLED.print(t);
      myOLED.print(" C");
      Serial.print("Влажность: ");
      Serial.print(h);
      Serial.print("%\t");
      Serial.print("Температура ");
      Serial.print(t);
      Serial.println(" *C ");

    
}
