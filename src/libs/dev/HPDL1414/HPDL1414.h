#ifndef HPDL1414_H
#define HPDL1414_H

// local includes
#include "Arduino.h"

class HPDL1414
{
    public:
        HPDL1414(uint32_t wr, uint32_t a0, uint32_t a1, uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6);
        ~HPDL1414();
        void init();
        void blank();
        void setDigit(uint8_t pos, char c);
        
    private:
        void resetPins();
        void setData(uint8_t data);
        void setAddress(uint8_t address);

        uint32_t _wr;
        uint32_t _a[2];
        uint32_t _d[7];

        bool _initialized;

};

#endif