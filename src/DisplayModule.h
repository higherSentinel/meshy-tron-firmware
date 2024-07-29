// parent class
#ifndef DISPLAY_MODULE_H
#define DISPLAY_MODULE_H

// local includes
#include "Arduino.h"
#include "../task-info.h"
#include <Logger.h>
#include "libs/generators/LinearRampFF/LinearRampFF.h"

// local defines
#define MAX_NO_OF_DISPLAY_MODULES 4
#define MIN_PULSE_TIME_MS 0x200  // due to FF ramp

class DisplayModule
{
    public:
        DisplayModule() = default;
        ~DisplayModule() = default;
        static TaskHandle_t * dispmod_tsk_handle;
        virtual void update()= 0; // force the children... hahaha >)
        static uint8_t initModule(uint32_t pulse_period_ms);
        static void run(void * params);
        static void addDisplay(DisplayModule * display);
    
    private:
        static uint32_t _frame_time;
        static bool _initialized;
        static DisplayModule* _displays[MAX_NO_OF_DISPLAY_MODULES];
        static uint8_t _dcount;
};

#endif