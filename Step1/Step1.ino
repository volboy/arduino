#include <StepMotor.h>
#include <MsTimer2.h>

StepMotor myMotor(10,11,12,13); //создаем объект типа StepMotor, задаем вывода для фаз
unsigned int timeCounter; //счетчик времени
byte md; // режим: 0-вращение против чс, 1-пауза, 2-вращение по чс, 3-пауза

void setup() {
  MsTimer2::set(1,timerInterrupt); //задаем период прерывания по таймеру 1 мс
  MsTimer2::start(); //разрещаем прерывание по таймеру
  myMotor.setMode(0, false); //шаговый режим, без фиксации при остановке
  myMotor.setDivider(2); //делитель частоты 21 (при прерывании 1 мс период коммутации фаз 21 мс)
  md=0; //начальный режим
  myMotor.step(10); //начальный запуск
  myMotor.step(-10);
}

void loop() {
  // управление вращением двигателя
  if (md==0) {
    //пять оборотов против часовой стрелки
    if (myMotor.readSteps()==0) {md=1; timeCounter=0;}
  }
  else if (md==1) {
    //пауза 1 сек
    if (timeCounter>=1000) {md=2; myMotor.step(-120);}
  }
  else if (md==2) {
    //пять оборотов по часовой стрелке
    if (myMotor.readSteps()==0) {md=3; timeCounter=0;}
  }
  else {
    //пауза 1 сек
    if (timeCounter>=1000) {md=0; myMotor.step(120);}
  }
}
//обработчик прерывания 1 мс
void timerInterrupt(){
  myMotor.control(); //управление двигателем
  timeCounter++; //счетчик времени
}
