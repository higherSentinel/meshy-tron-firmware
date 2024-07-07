#ifndef SRNIXIE_H
#define SRNIXIE_H

// local includes
#include "Arduino.h"
#include "../SR595/SR595.h"


// wrapper class for the shift register based nixie tubes
class SRnixie
{
    // LUT for sr to nixie pin map       |   0  1  2  3  4  5  6   7   8   9  .
    static constexpr uint8_t srnixpm[11] = {14, 1, 2, 3, 4, 5, 6, 7, 12, 13, 15};

    public:
        SRnixie();
        ~SRnixie();
        void init(SR595 *  sr_instance, uint32_t bpin);
        void setDigit(uint8_t digit);
        void latch();
        void setBrightness(uint8_t brightness);

    private:
        SR595 * _sr_instance;
        bool _initialized = false;
        uint32_t _bpin;
};

#endif