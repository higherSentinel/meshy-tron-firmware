#include "SRnixie.h"

/**
 * @brief constructor
 */
SRnixie::SRnixie() 
{

}

/**
 * @brief destructor
 */
SRnixie::~SRnixie()
{

}

/**
 * @brief function to be called first from app
 */
void SRnixie::init(SR595 * sr_instance, uint32_t bpin)
{
    if(sr_instance == nullptr)
        return;

    _sr_instance = sr_instance;
    _sr_instance->init();
    _bpin = bpin;
    setBrightness(0);
    _initialized = true;
}

/**
 * @brief function to set the requested digit on the nixie
 * @param digit 0-10 (u8), 10 is the . on the nixie
 * @warning this fuunction will only clock data out to the SR.
 * the caller will need to use the latch call to latch out the
 * written data.
 */
void SRnixie::setDigit(uint8_t digit)
{
    if(!_initialized || digit > 11)
        return;

    _sr_instance->clearAll();
    _sr_instance->setBit(srnixpm[digit]);
    _sr_instance->clockOut();
}


/**
 * @brief function call to latch out data
 */
void SRnixie::latch()
{
    if(!_initialized)
        return;

    _sr_instance->latchOut();
}

/**
 * @brief call to set the max output brightness of the nixie
 * @param maxb (double) 0-1
 */
void SRnixie::setMaxBrightness(double maxb)
{
    // sec chk
    maxb = maxb > 1? 1 : maxb;
    maxb = maxb < 0? 0 : maxb;

    _maxb = maxb;
}

/**
 * @brief call to set the min output brightness of the nixie
 * @param minb (double) 0-1
 */
void SRnixie::setMinBrightness(double minb)
{
    // sec chk
    minb = minb > 1? 1 : minb;
    minb = minb < 0? 0 : minb;

    _minb = minb;
}



/**
 * @brief function call to set the nixie brightness 
 * @param brightness 0-255 (u8) PWM
 */
void SRnixie::setBrightness(uint8_t brightness)
{
    if(!_initialized)
        return;

    analogWrite(_bpin, (0xFF - (uint8_t)(_maxb*brightness)));
}