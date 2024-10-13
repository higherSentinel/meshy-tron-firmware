#ifndef SEPLED_H
#define SEPLED_H

// local includes
#include "Arduino.h"
#include "../../sw-modules/DisplayModule/DisplayModule.h"

// local defines
#define SEPLED_MIN_PULSE_TIME_MS    0x200

// it ain't much but a god damn wrapper for an LED
class SeparatorLED : DisplayFE
{
    public:
        static SeparatorLED& getInstance();
        void update();
        ~SeparatorLED();
        void init(uint32_t pin, double maxb);
        void setBrightness(uint8_t brightness);
        

    private:
        SeparatorLED();
        uint32_t _pin;
        bool _initialized = false;
        double _maxb;
};

#endif