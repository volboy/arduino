#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include <MsTimer2.h>
#include <avr/wdt.h>




#define DHTPIN 2 //сигнальный пин с датчика влажности
#define POWERON 6 //сигнал включения контроллера
DHT dht(DHTPIN, DHT11); //инициализация датчика
LiquidCrystal_I2C lcd(0x3F, 16, 2); //инициализация дисплея с адресом по шине I2C
volatile int timerON=0;
volatile int timerDHT=0;
boolean flagOFF=false;
float h=0.0, t=0.0;



void setup() {
 
  dht.begin();
  lcd.init();
  lcd.setCursor(0,0);
  lcd.backlight();
  pinMode(POWERON, OUTPUT);
  MsTimer2::set(2, timerInterupt);
  MsTimer2::start();
  wdt_enable(WDTO_1S);

    }

void loop() {
 
  if (flagOFF==false) {
    digitalWrite(POWERON, HIGH);
  }

  if (timerDHT>1500) {
    timerDHT=0;
  
  h=dht.readHumidity(); 
  t=dht.readTemperature();
    
  if (isnan(h)||isnan(t)) { //проверка, если не удается считать данные то вывести ошибка считывания

    lcd.print("ERROR OF SENSOR");
    return;
      }
      
      lcd.setCursor(0,0);
      lcd.print("h=");
      lcd.print(h);
      lcd.print(" %");
      lcd.setCursor(0,1);
      lcd.print("t=");
      lcd.print(t);
      lcd.print(" C");
  }      
}

void timerInterupt(){
  
  timerON++;
  
  timerDHT++;
  if (timerON>=6000){timerON=0; digitalWrite(POWERON, LOW); flagOFF=true;}
  wdt_reset();
  
  }
