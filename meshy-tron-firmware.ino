#include "meshy-tron-firmware.h"

SR595 sr1(NIXIE_SR_CLK_1, NIXIE_SR_COM_LAT, NIXIE_SR_COM_DAT, sr595_type_2x);
SR595 sr2(NIXIE_SR_CLK_2, NIXIE_SR_COM_LAT, NIXIE_SR_COM_DAT, sr595_type_2x);
SR595 sr3(NIXIE_SR_CLK_3, NIXIE_SR_COM_LAT, NIXIE_SR_COM_DAT, sr595_type_2x);
SR595 sr4(NIXIE_SR_CLK_4, NIXIE_SR_COM_LAT, NIXIE_SR_COM_DAT, sr595_type_2x);

void setup()
{
  // setup comms port & logger
  Serial.begin(115200);
  Logger::setLogLevel(LOGGER_LEVEL);

  // init on-board led
  LED_INIT;
  LED_OFF;

  sr1.init();
  sr2.init();
  sr3.init();
  sr4.init();

  // MR
  pinMode(NIXIE_SR_COM_MR, INPUT);

  Logger::verbose("SETUP COMPLETE");
}

uint8_t tog = 0x00;
uint8_t tog2 = 0x00;
uint8_t digib = 1;
uint16_t h = 0;

void loop()
{
  if(tog == 0)
  {
    sr1.clearAll();
    sr1.setBit(digib);
    sr1.clockOut();

    sr2.clearAll();
    sr2.setBit(digib);
    sr2.clockOut();
    
    sr3.clearAll();
    sr3.setBit(digib);
    sr3.clockOut();
    
    sr4.clearAll();
    sr4.setBit(digib++);
    sr4.clockOut();
    sr4.latchOut();

    digib = digib == 8? 12 : digib;
    digib = digib >= 16? 1 : digib;
  }

  delay(5);
  analogWrite(NIXIE_PWM_1, 0xff-tog);
  analogWrite(NIXIE_PWM_2, 0xff-tog);
  analogWrite(NIXIE_PWM_3, 0xff-tog);
  analogWrite(NIXIE_PWM_4, 0xff-tog);
  analogWrite(SEPERATION_LED, tog);

  tog2 = tog == 0xFF? 1 : tog2;
  tog2 = tog == 0x00? 0: tog2;

  tog = tog2? tog - 1: tog + 1;
}