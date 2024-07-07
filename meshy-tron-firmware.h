// global includes
#include "Arduino.h"
#include <Logger.h>
#include <Adafruit_NeoPixel.h>
#include "src/libs/dev/SR595/SR595.h"
#include "src/libs/dev/HPDL1414/HPDL1414.h"
#include "src/libs/dev/SRnixie/SRnixie.h"
#include "src/libs/dev/SeparatorLED/SeparatorLED.h"

// global defines
#define LEDPIN              48
#define SERIAL_BAUD         115200
#define LOGGER_LEVEL        Logger::VERBOSE

// modules
// #define ENABLE_NIXIE // comment to disable all nixies
#define ENABLE_NIXIE_1 // comment to disable nixie 1
#define ENABLE_NIXIE_2 // comment to disable nixie 2
#define ENABLE_NIXIE_3 // comment to disable nixie 3
#define ENABLE_NIXIE_4 // comment to disable nixie 4

#define ENABLE_MINI_DISPLAY // comment to disable the mini hpdl1414 display

// #define ENABLE_SEPARATOR_LED // comment to disable the seperator led

// animation timing
#define FRAME_RATE          240.00 //fps
#define FRAME_TIME          (1000.00/FRAME_RATE)

// inbuilt RGB led object & macro fuctions
Adafruit_NeoPixel             obled(1, LEDPIN, NEO_GRB + NEO_KHZ800);
#define LED_INIT              obled.begin()
#define LED_SET_RGB(R, G, B)  obled.setPixelColor(0, obled.Color(R, G, B))
#define LED_SET_HSV(H, S, V)  obled.setPixelColor(0, obled.ColorHSV(H, S, V))
#define LED_SHOW              obled.show()
#define LED_OFF               obled.clear(); \
                              obled.show()

// hpdl1414
#define HPDL1414_A0             3       // needs to be floating on boot
#define HPDL1414_A1             42      // NO JTAG       
#define HPDL1414_D0             41      // NO JTAG
#define HPDL1414_D1             40      // NO JTAG
#define HPDL1414_D2             39      // NO JTAG
#define HPDL1414_D3             38
#define HPDL1414_D4             18      // shared with main nixie sr data pin
#define HPDL1414_D5             14
#define HPDL1414_D6             48
#define HPDL1414_WR             2


// nixie related
#define NIXIE_SR_COM_MR         21
#define NIXIE_SR_COM_DAT        18      // shared with hpdl1414 D4
#define NIXIE_SR_COM_LAT        17
#define NIXIE_SR_CLK_1          4
#define NIXIE_SR_CLK_2          5
#define NIXIE_SR_CLK_3          6
#define NIXIE_SR_CLK_4          7
#define NIXIE_PWM_1             10
#define NIXIE_PWM_2             11
#define NIXIE_PWM_3             12
#define NIXIE_PWM_4             13

// misc
#define SEPARATION_LED          47
#define RTC_INT                 1