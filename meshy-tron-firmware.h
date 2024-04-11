// global includes
#include "Arduino.h"
#include <Logger.h>
#include <Adafruit_NeoPixel.h>
#include "src/libs/dev/SR595/SR595.h"

// global defines
#define LEDPIN              48
#define SERIAL_BAUD         115200
#define LOGGER_LEVEL        Logger::VERBOSE

// inbuilt RGB led object & macro fuctions
Adafruit_NeoPixel             obled(1, LEDPIN, NEO_GRB + NEO_KHZ800);
#define LED_INIT              obled.begin()
#define LED_SET_RGB(R, G, B)  obled.setPixelColor(0, obled.Color(R, G, B))
#define LED_SET_HSV(H, S, V)  obled.setPixelColor(0, obled.ColorHSV(H, S, V))
#define LED_SHOW              obled.show()