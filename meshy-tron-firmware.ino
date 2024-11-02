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

// struct for the nixie front end display
nixie_frontend_t fe_data;


// isr for the minute-by-minute update
void minuteISR()
{
  // minute_int_flag = true;
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

WiFiUDP udp_ins;

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

  // start the nixie front end
  // make the struct to be shared
  fe_data.nixies[0] = &nixie[3];
  fe_data.nixies[1] = &nixie[2];
  fe_data.nixies[2] = &nixie[1];
  fe_data.nixies[3] = &nixie[0];
  fe_data.latch_pin = NIXIE_SR_COM_LAT;
  NixieFrontend::getInstance().init(&fe_data);
  NixieFrontend::getInstance().setFadeType(nixie_fade_animation_linear);
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
  MiniDisplay::getInstance().init(&md);
  #endif

  // init separator LED
  #ifdef ENABLE_SEPARATOR_LED
  SeparatorLED::getInstance().init(SEPARATION_LED_PIN, SEPARATOR_MAX_BRIGHTNESS);
  #endif

  // start modules
  DisplayModule::getInstance().setPulsePeriod(DISPLAY_MODULE_INTERVAL_MS);
  DisplayModule::getInstance().initModule();

  // start I2C bus
  Wire.begin();

  // connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  MiniDisplay::getInstance().setAnimation(&animation_knight_rider_dot);

  while(WiFi.status() != WL_CONNECTED)
  {
    // busy - wait
    delay(100);
  }

  MiniDisplay::getInstance().setText("WIFI");
  delay(1000);
  MiniDisplay::getInstance().setText("CONN");
  delay(1000);
  MiniDisplay::getInstance().blank();

  // setup ntp
  SNTPClient::getInstance().init(&udp_ins);

  // start RTC
  if(!startRTC())
  {
    Logger::verbose("SETUP: ", "RTC INIT FAIL");
    HALT;
  }

  Logger::verbose("- SETUP COMPLETE -");
}

mini_display_animation_t* animations[7] = 
{
  &animation_loading_star,
  &animation_knight_rider_dot,
  &animation_knight_rider_dash,
  &animation_fwrdbkwrd,
  &animation_wave,
  &animation_cart_wheel,
  &animation_rolling_plank,
};

uint64_t ntp_et = 0;
uint8_t a_count = 0;
uint16_t mcount = 1;
uint8_t nixie_cur_digits[4];
uint8_t nixie_digits[4];
uint32_t n_et = 0;
bool update_digits = false;

void loop()
{

  // manual trigger
  if(millis() > (n_et + 8000))
  {
    minute_int_flag = true;
    n_et = millis();
  }
    
  // update displays
  if(minute_int_flag)
  {
    mcount = mcount<<1;
    mcount = mcount == 0? 1 : mcount;
    sprintf(strbuf, "%4d", mcount);
    // MiniDisplay::getInstance().setText(strbuf);
    MiniDisplay::getInstance().setAnimation(animations[a_count++]);
    a_count %= 7;
    NixieFrontend::getInstance().setText(strbuf);
    minute_int_flag = false;
    DS3231::getInstance().clearAlarmFlag(Alarm2);
  }

  // ntp once
  if(ntp_et != 1)
  {
    if(SNTPClient::getInstance().reqNTPtime())
    {
      ntp_et = 1;
    } 
    delay(10);
  }
  if(ntp_et == 1)
  {
    // synced and internally tracked with millis
      uint32_t epoch = SNTPClient::getInstance().getEpoch();
      sprintf(strbuf, "ephoch = %ld", epoch);
      Logger::verbose("SNTP", strbuf);
      delay(1000);
  }

  // sprintf(strbuf, "UPTIME (ms, u32): %d", millis());
  // Logger::verbose("LOOP: ", strbuf);
}