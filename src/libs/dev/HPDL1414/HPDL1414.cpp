#include "HPDL1414.h"

/**
 * @brief constructor
 */
 HPDL1414::HPDL1414(uint32_t wr, uint32_t a0, uint32_t a1, uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6): _wr(wr) ,_a{a0, a1}, _d{d0, d1, d2, d3, d4, d5, d6} 
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
void HPDL1414::init()
{
    // disable write first
    pinMode(_wr, OUTPUT);
    digitalWrite(_wr, HIGH);

    // setup address and then data pins

    for(uint8_t i = 0; i < 2; i++)
    {
        pinMode(_a[i], OUTPUT);
        digitalWrite(_a[i], LOW);
    }


    for(uint8_t i = 0; i < 7; i++)
    {
        pinMode(_d[i], OUTPUT);
        digitalWrite(_d[i], LOW);
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
        digitalWrite(_d[i], (data & (0x01 << i)) != 0? HIGH : LOW);
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
        digitalWrite(_a[i], (address & (0x01 << i)) != 0? HIGH : LOW);
    }
}

/**
 * @brief internal function call to reset all pins to default
 */
void HPDL1414::resetPins()
{
    digitalWrite(_wr, HIGH);
    for(uint8_t i = 0; i < 2; i++)
    {
        digitalWrite(_a[i], LOW);
    }
    
    for(uint8_t i = 0; i < 7; i++)
    {
        digitalWrite(_d[i], LOW);
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
    digitalWrite(_wr, LOW);
    delayMicroseconds(1);
    digitalWrite(_wr, HIGH);
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
        digitalWrite(_wr, LOW);
        delayMicroseconds(1);
        digitalWrite(_wr, HIGH);  
    }
    resetPins();
}