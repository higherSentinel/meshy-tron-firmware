// global includes
#include "Arduino.h"
#include <Logger.h>
#include <Adafruit_NeoPixel.h>
#include "src/libs/dev/SR595/SR595.h"
#include "src/libs/dev/HPDL1414/HPDL1414.h"
#include "src/libs/dev/SRnixie/SRnixie.h"
#include "src/libs/dev/SeparatorLED/SeparatorLED.h"
#include "src/libs/dev/DS3231/DS3231.h"
#include "src/libs/hw-modules/NixieFrontend/NixieFrontend.h"
#include "src/libs/hw-modules/MiniDisplay/MiniDisplay.h"
#include <Wire.h>
#include "board-def.h"
#include "task-info.h"

// global defines
#define SERIAL_BAUD         115200
#define LOGGER_LEVEL        Logger::VERBOSE

// modules
#define ENABLE_NIXIE // comment to disable all nixies
#define ENABLE_NIXIE_1 // comment to disable nixie 1
#define ENABLE_NIXIE_2 // comment to disable nixie 2
#define ENABLE_NIXIE_3 // comment to disable nixie 3
#define ENABLE_NIXIE_4 // comment to disable nixie 4
#define NIXIE_MAX_BRIGHTNESS        0.5 // 0-1
#define NIXIE_MIN_BRIGHTNESS        0.0 // 0-1

#define ENABLE_MINI_DISPLAY // comment to disable the mini hpdl1414 display

#define ENABLE_SEPARATOR_LED // comment to disable the seperator led
#define SEPARATOR_MAX_BRIGHTNESS    0.25 // 0-1


// animation timing
#define FRAME_RATE          256.00 //fps
#define FRAME_TIME          (1000.00/FRAME_RATE)

// sw-module params
#define DISPLAY_MODULE_INTERVAL_MS       3072
