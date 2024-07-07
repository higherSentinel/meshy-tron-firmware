#ifndef SEPLED_H
#define SEPLED_H

// local includes
#include "Arduino.h"

// it ain't much but a god damn wrapper for an LED
class SeparatorLED
{
    public:
        static SeparatorLED& getInstance();
        ~SeparatorLED();
        void init(uint32_t pin);
        void setBrightness(uint8_t brightness);

    private:
        SeparatorLED();
        uint32_t _pin;
        bool _initialized = false;
};

#endif