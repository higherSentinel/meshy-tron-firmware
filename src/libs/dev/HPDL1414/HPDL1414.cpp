#include "HPDL1414.h"

/**
 * @brief constructor
 */
 HPDL1414::HPDL1414() 
{

}

/**
 * @brief destructor
 */
HPDL1414::~HPDL1414()
{

}

/**
 * @brief function to be called first from app
 */
void HPDL1414::init(hpdl1414_pins_t * pdef)
{
    memcpy(&_pdef, pdef, sizeof(hpdl1414_pins_t));

    // disable write first
    pinMode(_pdef.wr, OUTPUT);
    digitalWrite(_pdef.wr, HIGH);

    // setup address and then data pins

    for(uint8_t i = 0; i < 2; i++)
    {
        pinMode(_pdef.a[i], OUTPUT);
        digitalWrite(_pdef.a[i], LOW);
    }


    for(uint8_t i = 0; i < 7; i++)
    {
        pinMode(_pdef.d[i], OUTPUT);
        digitalWrite(_pdef.d[i], LOW);
    }

    _initialized = true;
}

/**
 * @brief internal function to set data
 * @param data char to set between 32 & 95
 */
void HPDL1414::setData(uint8_t data)
{
    for(uint8_t i = 0; i < 7; i++)
    {
        digitalWrite(_pdef.d[i], (data & (0x01 << i)) != 0? HIGH : LOW);
    }
}

/**
 * @brief internal function to set the address
 * @param address digit to set 0-3
 */
void HPDL1414::setAddress(uint8_t address)
{
    for(uint8_t i = 0; i < 2; i++)
    {
        digitalWrite(_pdef.a[i], (address & (0x01 << i)) != 0? HIGH : LOW);
    }
}

/**
 * @brief internal function call to reset all pins to default
 */
void HPDL1414::resetPins()
{
    digitalWrite(_pdef.wr, HIGH);
    for(uint8_t i = 0; i < 2; i++)
    {
        digitalWrite(_pdef.a[i], LOW);
    }
    
    for(uint8_t i = 0; i < 7; i++)
    {
        digitalWrite(_pdef.d[i], LOW);
    }
}

/**
 * @brief function call to set a char to a digit
 * @param pos digit position
 * @param c charcter to set in-between 32 & 95
 */
void HPDL1414::setDigit(uint8_t pos, char c)
{
    if(!_initialized)
        return;

    // bound
    c &= 0x7F;
    pos &= 0x03;

    resetPins();
    setAddress(pos);
    setData(c);
    digitalWrite(_pdef.wr, LOW);
    delayMicroseconds(1);
    digitalWrite(_pdef.wr, HIGH);
    resetPins();
}

/**
 * @brief function call to blank the display
 */
void HPDL1414::blank()
{
    if(!_initialized)
        return;

    resetPins();
    setData(' ');
    for(uint8_t i = 0; i < 4; i++)
    {
        setAddress(i);
        digitalWrite(_pdef.wr, LOW);
        delayMicroseconds(1);
        digitalWrite(_pdef.wr, HIGH);  
    }
    resetPins();
}