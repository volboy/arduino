#include <MsTimer2.h>

#define TIME_OUT 6 //время таймаута приема команды (мс)
float voltage; //напряжение
byte waterDog=0x0; //младший бит протечка воды
int timeCount; //счетчик времени
byte dataSerialBuf[10]; // буфер данных для передачи
byte  timeOutCount; // счетчик времени тайм-аута пакета
byte n; // число принятых байтов


void setup() {
  MsTimer2::set(1, timerInterrupt); //прерывания по таймеру период 1 мс
  MsTimer2::start(); //разрешение прерывания по таймеру
  Serial.begin(9600); 
  pinMode(13, OUTPUT);
  pinMode(2, INPUT);
}

void loop() {
  if (timeCount>200) {
    timeCount=0;
    voltage=(float)(analogRead(A0))*5./1024.;
    if (digitalRead(2)==true){
    waterDog=0x1;}
    else
    {waterDog=0x0;}
  }
  noInterrupts();  
  * (float *)(dataSerialBuf+4) = voltage;
  *(byte*)dataSerialBuf=waterDog;
  interrupts(); 

}

void timerInterrupt(){
  timeCount++;
  timeOutCount++;
  n=Serial.available(); //число принятых байт
  if (n==0) timeOutCount=0; //данных нет
  else if (n==3) {
    //принята команда от другого устройства, 3 байта
    //чтение команды в буфер
    byte buf[3];
    buf[0]=Serial.read(); //код операции + адрес контроллера (0х10)
    buf[1]=Serial.read(); //состояние светодиода (младший бит)
    buf[2]=Serial.read(); //контрольная сумма (байт0^байт1^0xe5)
    //проверка
    if ((buf[0]==0x10) && (buf[0]^buf[1]^0xe5)==buf[2]){
    //правильно
    if ((buf[1]&1)==0) digitalWrite(13,LOW); //управление светодиодом
    else digitalWrite(13, HIGH);

    //Ответ
    unsigned int sum=0; //контрольная сумма
    for (int i=0; i<10; i++) {
      Serial.write(dataSerialBuf[i]);
      sum+=dataSerialBuf[i];
    }
    //контрольная сумма ответа
    sum ^=0xa1e3;
    Serial.write(*((byte*)(&sum)));
    Serial.write(*(((byte*)(&sum))+1));
  }
    else {
      // не правильно
      // сброс порта
      timeOutCount= 0;
      while (true) { if (Serial.read() == 0xffff) break;}            
    }        
  }

  else if (n > 3) {
    // принято больше данных, не правильно
      // сброс порта
      timeOutCount= 0;
      while (true) { if (Serial.read() == 0xffff) break;}                
  }

  else {
    // не все байты приняты
    // проверка тайм-аута
    if (timeOutCount > TIME_OUT) {
      // сброс порта
      timeOutCount= 0;
      while (true) { if (Serial.read() == 0xffff) break;}      
    }                        
  }   
}
