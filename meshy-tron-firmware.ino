#include "meshy-tron-firmware.h"

void setup()
{
  // setup comms port & logger
  Serial.begin(115200);
  Logger::setLogLevel(LOGGER_LEVEL);

  // init on-board led
  LED_INIT;

  Logger::verbose("SETUP COMPLETE");
}

uint16_t h = 0;

void loop()
{
  LED_SET_HSV(h++, 0xff, 0xf0);
  LED_SHOW;
  delayMicroseconds(1);
}
