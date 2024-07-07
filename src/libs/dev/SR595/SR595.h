#ifndef SR_74_595_H
#define SR_74_595_H

// local Includes
#include "Arduino.h"

// local defines
#define SR595_MAX_SR_SIZE 0x08;


// SR type, 8 if 1x 595 or 16 if 2x 595. lib only supports 2 in series.
typedef enum 
{
    sr595_type_1x = 1,
    sr595_type_2x,
} sr595_type_t;


class SR595
{
    public:
        SR595(uint32_t clk_pin, uint32_t reg_out_pin, uint32_t data_pin, sr595_type_t sr_type);
        ~SR595();
        
        void clockOut();
        void setBit(uint8_t pos);
        void clearBit(uint8_t pos);
        void clearAll();
        void setAll();
        void latchOut();
        void init();

    private:
        uint32_t _clk_pin;
        uint32_t _reg_out_pin;
        uint32_t _data_pin;
        sr595_type_t _sr_type;
        uint8_t _max_pos;
        bool _initialized = false;
        uint16_t _reg_data = 0;


};

#endif