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
  * @param pin
  */
 void SeparatorLED::init(uint32_t pin, double maxb)
 {
    // assign the private members and calculate the animation frame time
    _pin = pin;
    _maxb = maxb > 1? 1 : maxb;
    _maxb = _maxb < 0? 0 : _maxb;

   // init & set default
   pinMode(_pin, OUTPUT);
   digitalWrite(_pin, 0);

   // add self to display module
   addDisplay(this);

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
    
    analogWrite(_pin, brightness*_maxb);
 }

 /**
  * @brief display module update call
 */
void SeparatorLED::update()
{
    setBrightness(LinearRampFF::getSample());
}