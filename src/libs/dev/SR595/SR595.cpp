#include "SR595.h"

/**
 * @brief Constructor
 * @param clk_pin       mcu pin connected to the data clock pin on the 74HC595
 * @param reg_out_pin   mcu pin connected to the register latch pin on the 74HC595
 * @param data_pin      mcu pin connected to the data input pin on the 74HC595
 * @param sr_type       type of the sr configuration, 2x if 2 74HC595s are connected in series
 */
 SR595::SR595(uint32_t clk_pin, uint32_t reg_out_pin, uint32_t data_pin, sr595_type_t sr_type)
 {
    _clk_pin = clk_pin;
    _reg_out_pin = reg_out_pin;
    _data_pin = data_pin;
    _sr_type = sr_type;

    _max_pos = (uint8_t)_sr_type * SR595_MAX_SR_SIZE;
 }
 
 /**
  * @brief Function call to init pins
  */
 void SR595::init()
 {
   // init all pins
   pinMode(_clk_pin, OUTPUT);
   pinMode(_reg_out_pin, OUTPUT);
   pinMode(_data_pin, OUTPUT);
   
   // set defaults
   digitalWrite(_clk_pin, 0);
   digitalWrite(_reg_out_pin, 0);
   digitalWrite(_data_pin, 0);
 }

 /**
  * @brief Destructor
  */
 SR595::~SR595()
 {
    // set pins to float
    pinMode(_clk_pin, INPUT);
    pinMode(_reg_out_pin, INPUT);
    pinMode(_data_pin, INPUT);
 }

 /**
  * @brief Function call to set a bit at the specified position
  * @param pos bit position. starts at 0
  */
 void SR595::setBit(uint8_t pos)
 {
    if(pos >= _max_pos)
        return;

    _reg_data |= (uint16_t)0x01 << pos;
 }

/**
 * @brief Function call to clear a bit at the specified position
 * @param pos bit position. starts at 0
 */
 void SR595::clearBit(uint8_t pos)
 {
    if(pos >= _max_pos)
        return;

    _reg_data &= ~((uint16_t)0x01 << pos);
 }

/**
 * @brief Function call to clear the whole register
 */
 void SR595::setAll()
 {
    _reg_data = 0xFF;
 }

 /**
 * @brief Function call to set the whole register
 */
 void SR595::clearAll()
 {
    _reg_data = 0x00;
 }

 /**
  * @brief Function call to set the data
  * @warning the fuction is blocking and consumes 2us per clock
  */
 void SR595::clockOut()
 {
   for(uint8_t i = 0; i < _max_pos; i++)
   {
      digitalWrite(_data_pin, _reg_data & (0x01 << (_max_pos-i-1)));
      digitalWrite(_clk_pin, !0);
      delayMicroseconds(1);
      digitalWrite(_clk_pin, 0);
      delayMicroseconds(1);
   }
   digitalWrite(_data_pin, 0);
 }

/**
 * @brief Function call to latch the data to the output
 * @warning the fuction is blocking and consumes 1us
 */
 void SR595::latchOut()
 {
    digitalWrite(_reg_out_pin, !0);
    delayMicroseconds(1);
    digitalWrite(_reg_out_pin, 0);
 }