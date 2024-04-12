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
#define LED_OFF               obled.clear(); \
                              obled.show()


// pin definitions

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
#define SEPERATION_LED          47
#define RTC_INT                 1