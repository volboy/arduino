
#include <SPI.h>
#include <UIPEthernet.h>

//определеяем конфигурацию сети
byte mac[]={0xAE, 0xB2, 0x26, 0xE4, 0x4A, 0x5C}; //MAC-адрес
byte ip[]={192, 168, 1, 10}; //IP-адрес
byte myDns[]={192, 168, 1, 1}; // адрес DNS сервера
byte gateway[]={192, 168, 1, 1}; //адрес сетевого шлюза
byte subnet[]={255, 255, 255, 0}; //маска подсети
byte myDigitalPins=00000000; ;
EthernetServer server(2000); //создаем сервер, порт 2000
EthernetClient client; //обьект клиент
boolean clientAlreadyConnected=false; // признак клиент уже подключен

void setup() {
  Ethernet.begin(mac, ip, myDns, gateway, subnet); //инициализация контроллера
  server.begin(); //включаем ожидание входящих соединений
  Serial.begin(9600);
  Serial.print("Server address:");
  Serial.print(Ethernet.localIP()); //выводим IP-адрес контроллера
  pinMode(0, INPUT); 
  pinMode(1, INPUT); 
  pinMode(2, INPUT); 
  pinMode(3, INPUT); 
  pinMode(4, INPUT); 
  pinMode(5, INPUT); 
  pinMode(6, INPUT); 
  pinMode(7, INPUT); 
  pinMode(8, INPUT); 
  pinMode(9, INPUT);
}

void loop() {
  client=server.available(); //ожидаем объект клиент
  if (client) {
    // есть данные от клиента
    if (clientAlreadyConnected==false) {
      // сообщение о подключении
      Serial.println("Client connected");
      client.write("Server ready"); //Ответ клиенту
      clientAlreadyConnected=true;
    }

    while (client.available()>0) {
 
      char chr=client.read(); //чтение символа
        if (chr=='0') {
          
        myDigitalPins=00000000;       
        if (digitalRead(2)==true) {bitWrite(myDigitalPins, 0, HIGH);} 
        if (digitalRead(3)==true) {bitWrite(myDigitalPins, 1, HIGH);}
        if (digitalRead(4)==true) {bitWrite(myDigitalPins, 2, HIGH);}
        if (digitalRead(5)==true) {bitWrite(myDigitalPins, 3, HIGH);}
        if (digitalRead(6)==true) {bitWrite(myDigitalPins, 4, HIGH);}
        if (digitalRead(7)==true) {bitWrite(myDigitalPins, 5, HIGH);}
        if (digitalRead(8)==true) {bitWrite(myDigitalPins, 6, HIGH);}
        if (digitalRead(9)==true) {bitWrite(myDigitalPins, 7, HIGH);}
             
      }
       
     
      server.write(myDigitalPins);
      Serial.write(chr);
      
    }
  }
  
}
