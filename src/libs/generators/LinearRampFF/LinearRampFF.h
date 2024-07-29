#ifndef LIN_RAMP_H
#define LIN_RAMP_H

// local include
#include "Arduino.h"

class LinearRampFF
{
    public:
        // external call to advance ramp
        static void advRamp()
        {
            _sample = (((_psc & 0x100)>>8) * (0xFF & ~_psc)) + ((((_psc & 0x100)>>8)^0x01) * (0xFF & _psc));
            _psc++;
            _psc&=~0xFE00;
        }

        // call to get the current sample
        static uint8_t getSample()
        {
            return _sample;
        }

        // call to know if gradient is negative
        static bool isGradNeg()
        {
            return ((_psc & 0x100)>>8)? true : false;
        }

    private:
        // Singleton
        LinearRampFF();
        static inline uint16_t _psc = 0;
        static inline uint8_t _sample = 0;
};
#endif