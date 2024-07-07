#include "SeparatorLED.h"

/**
 * @brief singleton
 */
SeparatorLED& SeparatorLED::getInstance()
{
    static SeparatorLED instance;
    return instance;
}

/**
 * @brief constructor
 */
 SeparatorLED::SeparatorLED()
 {

 }
 
/**
 * @brief destructor
 */
SeparatorLED::~SeparatorLED()
{

}

 /**
  * @brief Function call to init pins
  */
 void SeparatorLED::init(uint32_t pin)
 {
    _pin = pin;

   // init & set default
   pinMode(_pin, OUTPUT);
   digitalWrite(_pin, 0);

   _initialized = true;
 }


 /**
  * @brief Function call to set brightness
  * @param brightness 0-255 (u8)
  */
 void SeparatorLED::setBrightness(uint8_t brightness)
 {
    if(!_initialized)
        return;
    
    analogWrite(_pin, brightness);
 }