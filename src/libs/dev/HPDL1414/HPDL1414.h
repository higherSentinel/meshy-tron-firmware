#ifndef HPDL1414_H
#define HPDL1414_H

// local includes
#include "Arduino.h"

typedef struct
{
    uint32_t wr; // write (active low)
    uint32_t a[2]; // address pins (digit select)
    uint32_t d[7]; // ascii i.e. data pins 
}hpdl1414_pins_t;

class HPDL1414
{
    public:
        HPDL1414();
        ~HPDL1414();
        void init(hpdl1414_pins_t * pdef);
        void blank();
        void setDigit(uint8_t pos, char c);
        void setText(char * str);
        
    private:
        void resetPins();
        void setData(uint8_t data);
        void setAddress(uint8_t address);

        hpdl1414_pins_t _pdef;

        bool _initialized;

};

#endif