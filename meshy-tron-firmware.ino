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

// clock sm stuff
bool clock_not_synced = false;
bool minute_int_flag = false;


// isr for the minute-by-minute update
void minuteISR()
{
  minute_int_flag = true;
}


bool startRTC()
{
    // set instance and verify that the RTC is on the bus
    if(!DS3231::getInstance().init(&Wire))
      return false;
    
    // start the clock if not started
    clock_not_synced = DS3231::getInstance().isActive();
    if(!clock_not_synced)
    {
      DS3231::getInstance().startClock();
    }

    // set alarm 2 to start on each minute
    DS3231::getInstance().setAlarm2(Alarm2EveryMinute, 0, 0, 0);

    // enable alarm 2 interrupt & pin
    attachInterrupt(RTC_INT_PIN, minuteISR, FALLING);
    DS3231::getInstance().alarmInterruptEnable(Alarm2, true);
}


void setup()
{
  // setup comms port & logger
  Serial.begin(SERIAL_BAUD);
  Logger::setLogLevel(LOGGER_LEVEL);

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

  // set all nixies to 0
  for(uint8_t i = 0; i < 4; i++)
  {
    nixie[i].setDigit(0);
    nixie[i].setBrightness(CONV_NIXIE_BRIGHTNES(NIXIE_MIN_BRIGHTNESS));
    nixie[i].updateNixie();
    nixie[i].setFade(sr_nixie_fade_linear);
  }
  // send out
  latchAllNixies();
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
  SeparatorLED::getInstance().init(SEPARATION_LED_PIN);
  #endif

  // start I2C bus
  Wire.begin();

  // start RTC
  if(!startRTC())
  {
    Logger::verbose("SETUP: ", "RTC INIT FAIL");
    HALT;
  }

  Logger::verbose("- SETUP COMPLETE -");
}

uint8_t tog = 0x00;
uint8_t tog2 = 0x00;
uint8_t asci = 32;
uint8_t digib = 0;
uint16_t h = 0;
uint64_t loop_et = 0;


uint64_t ps_et = 0;
uint16_t psc = 0;

uint16_t mcount = 1;

void pulseSeparator()
{
    SeparatorLED::getInstance().setBrightness(((psc & 0x100)? 0xFF & ~psc : 0xFF & psc) * SPERATOR_MAX_BRIGHTNESS);
    psc++;
    psc&=~0xFE00;
    ps_et = millis();
}

uint8_t nixie_cur_digits[4];
uint8_t nixie_digits[4];
bool update_digits = false;

void loop()
{
  // frame time, if not elapsed return
  if((loop_et + FRAME_TIME) > millis())
    return;
  
  // update displays
  pulseSeparator();
  
  // update nixies
  for(uint8_t i = 0; i < 4; i++)
  {
    nixie[i].updateNixie();
  }

  updateNixieDigits();

  if(minute_int_flag)
  {
    mcount = mcount<<1;
    sprintf(strbuf, "%4d", mcount);
    md.setText(strbuf);
    Logger::verbose("LOOP: ", strbuf);

    // find out which nixies need to be updated
    for(uint8_t i = 0; i < 4; i++)
    {
      nixie_digits[i] = strbuf[4-1-i];
      Serial.print("digit ");
      Serial.print(i);
      Serial.print("=");
      Serial.println((char)nixie_digits[i]);
      if((nixie_digits[i] != nixie_cur_digits[i]) || nixie_digits[i] == ' ')
      {
        nixie[i].setBrightness(CONV_NIXIE_BRIGHTNES(NIXIE_MIN_BRIGHTNESS));
      }
    }

    update_digits = true;
    minute_int_flag = false;
    DS3231::getInstance().clearAlarmFlag(Alarm2);
  }

  

  // sprintf(strbuf, "UPTIME (ms, u32): %d", millis());
  // Logger::verbose("LOOP: ", strbuf);

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

// external to update the nixie digits
void updateNixieDigits()
{
  if(update_digits)
  {
    // check if all the nixies have reached zero and update the
    for(uint8_t i = 0; i < 4; i++)
    {
      if(!nixie[i].isBrightnessSet())
        return;
    }

    // set digits to all of em since common latch
    for(uint8_t i = 0; i < 4; i++)
    {
      if((nixie_cur_digits[i]!=nixie_digits[i]) && nixie_digits[i] != ' ')
      {
          nixie[i].setBrightness(CONV_NIXIE_BRIGHTNES(NIXIE_MAX_BRIGHTNESS));
      }
      nixie_cur_digits[i] = nixie_digits[i];
      if(nixie_cur_digits[i] > 47 && nixie_cur_digits[i] < 58)
      {
        nixie[i].setDigit(nixie_cur_digits[i]-48);
      }
    }
  }

  // allof the nixies have reached zero brightness
  // send out updated digits & clear update flag
  latchAllNixies();
  update_digits = false;
}