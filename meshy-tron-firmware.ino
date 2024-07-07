#include "meshy-tron-firmware.h"

// nixie digit shift registers & wrapper class
SR595 sr1(NIXIE_SR_CLK_1, NIXIE_SR_COM_LAT, NIXIE_SR_COM_DAT, sr595_type_2x);
SR595 sr2(NIXIE_SR_CLK_2, NIXIE_SR_COM_LAT, NIXIE_SR_COM_DAT, sr595_type_2x);
SR595 sr3(NIXIE_SR_CLK_3, NIXIE_SR_COM_LAT, NIXIE_SR_COM_DAT, sr595_type_2x);
SR595 sr4(NIXIE_SR_CLK_4, NIXIE_SR_COM_LAT, NIXIE_SR_COM_DAT, sr595_type_2x);
SRnixie nixie[4];

// hpdl1414 & pdef stuct
HPDL1414 md;
hpdl1414_pins_t segdisppdef;

// buff for str ops
static char strbuf[0xFF];

void setup()
{
  // setup comms port & logger
  Serial.begin(115200);
  Logger::setLogLevel(LOGGER_LEVEL);

  // init on-board led
  LED_INIT;
  LED_OFF;

  // cycle MR
  // MR
  pinMode(NIXIE_SR_COM_MR, OUTPUT);
  digitalWrite(NIXIE_SR_COM_MR, LOW);
  pinMode(NIXIE_SR_COM_MR, INPUT); // external pu on pin

  // init nixies
  #ifdef ENABLE_NIXIE
  pinMode(NIXIE_SR_COM_LAT, OUTPUT);
  #ifdef ENABLE_NIXIE_1
  nixie[0].init(&sr1, NIXIE_PWM_1);
  #endif
  #ifdef ENABLE_NIXIE_2
  nixie[1].init(&sr2, NIXIE_PWM_2);
  #endif
  #ifdef ENABLE_NIXIE_3
  nixie[2].init(&sr3, NIXIE_PWM_3);
  #endif
  #ifdef ENABLE_NIXIE_4
  nixie[3].init(&sr4, NIXIE_PWM_4);
  #endif
  #endif

  // init mini-display
  #ifdef ENABLE_MINI_DISPLAY
  //setup hpdl struct
  segdisppdef.wr = HPDL1414_WR;
  segdisppdef.a[0] = HPDL1414_A0;
  segdisppdef.a[1] = HPDL1414_A1;
  segdisppdef.d[0] = HPDL1414_D0;
  segdisppdef.d[1] = HPDL1414_D1;
  segdisppdef.d[2] = HPDL1414_D2;
  segdisppdef.d[3] = HPDL1414_D3;
  segdisppdef.d[4] = HPDL1414_D4;
  segdisppdef.d[5] = HPDL1414_D5;
  segdisppdef.d[6] = HPDL1414_D6; 
  md.init(&segdisppdef);
  md.blank();
  #endif

  // init separator LED
  #ifdef ENABLE_SEPARATOR_LED
  SeparatorLED::getInstance().init(SEPARATION_LED);
  #endif

  Logger::verbose("SETUP COMPLETE");
}

uint8_t tog = 0x00;
uint8_t tog2 = 0x00;
uint8_t asci = 32;
uint8_t digib = 0;
uint16_t h = 0;
uint64_t loop_et = 0;

void loop()
{
  // frame time, if not elapsed return
  if((loop_et + FRAME_TIME) > millis())
    return;

  if(tog == 0)
  {
    sprintf(strbuf, "digit = %d", digib);
    Logger::verbose("LOOP", strbuf);

    nixie[0].setDigit(digib);
    nixie[1].setDigit(digib);
    nixie[2].setDigit(digib);
    nixie[3].setDigit(digib++);

    // manual latch use nixie 3 latch
    latchAllNixies();

    digib = digib > 10? 0 : digib;
  }

  for(uint8_t i = 0; i < 4; i++)
  {
    nixie[i].setBrightness(0xff-tog);
  }
  
  SeparatorLED::getInstance().setBrightness(tog);

  tog2 = tog == 0xFF? 1 : tog2;
  tog2 = tog == 0x00? 0: tog2;

  if(tog % 40 == 0)
  {
      md.setDigit(0, asci);
      md.setDigit(1, asci);
      md.setDigit(2, asci);
      md.setDigit(3, asci++);
      asci = asci >= 95? 32 : asci;
  }

  tog = tog2? tog - 1: tog + 1;

  sprintf(strbuf, "UPTIME (ms, u32)%d", millis());
  Logger::verbose("LOOP: ", strbuf);

  // update loop et for next frame
  loop_et = millis();
}

// external latch
void latchAllNixies()
{
    digitalWrite(NIXIE_SR_COM_LAT, !0);
    // delay not necessary but jic
    delayMicroseconds(1);
    digitalWrite(NIXIE_SR_COM_LAT, 0);
}