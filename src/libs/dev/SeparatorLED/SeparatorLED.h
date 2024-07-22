#ifndef SEPLED_H
#define SEPLED_H

// local includes
#include "Arduino.h"
#include "../../../../task-info.h"
#include <Logger.h>

// local defines
#define SEPLED_MIN_PULSE_TIME_MS    0x200
#define SEPLED_MAX_BRIGHTNESS       0.15 // 0-1

// it ain't much but a god damn wrapper for an LED
class SeparatorLED
{
    public:
        static SeparatorLED& getInstance();
        static void run(void * params);
        ~SeparatorLED();
        void init(uint32_t pin, uint32_t pulse_period_ms, double maxb);
        void setBrightness(uint8_t brightness);
        TaskHandle_t * sep_led_tsk_handle;

    private:
        SeparatorLED();
        uint32_t _pin;
        bool _initialized = false;
        uint32_t _frame_time;
        uint16_t _psc;
        double _maxb;
};

#endif