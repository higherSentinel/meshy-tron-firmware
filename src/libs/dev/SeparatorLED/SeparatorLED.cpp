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
  * @param pulse_period_ms min 512ms
  */
 void SeparatorLED::init(uint32_t pin, uint32_t pulse_period_ms, double maxb)
 {
    // assign the private members and calculate the animation frame time
    _pin = pin;
    _maxb = maxb > 1? 1 : maxb;
    _maxb = _maxb < 0? 0 : _maxb;

    uint32_t temp = pulse_period_ms < SEPLED_MIN_PULSE_TIME_MS? SEPLED_MIN_PULSE_TIME_MS : pulse_period_ms;
    _frame_time = temp / (double)SEPLED_MIN_PULSE_TIME_MS;

   // init & set default
   pinMode(_pin, OUTPUT);
   digitalWrite(_pin, 0);

    // try and start the thread
    if (xTaskCreate(SeparatorLED::run,  TASK_SEPRATOR_LED_PULSE_NAME, TASK_SEPRATOR_LED_PULSE_STACK_SIZE, NULL, TASK_SEPRATOR_LED_PULSE_PRIORITY, SeparatorLED::getInstance().sep_led_tsk_handle) != pdPASS)
    {
        Logger::error("SEPARATOR LED: ", "SEPERATOR LED TASK START FAIL");
        HALT;
    }

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
  * thread run
  */
 void SeparatorLED::run(void * params)
 {
    while(!0)
    {
        uint16_t psc = SeparatorLED::getInstance()._psc;
        SeparatorLED::getInstance().setBrightness((psc & 0x100)? 0xFF & ~psc : 0xFF & psc);
        psc++;
        psc&=~0xFE00;
        SeparatorLED::getInstance()._psc = psc;
        vTaskDelay(SeparatorLED::getInstance()._frame_time);
    }
 }