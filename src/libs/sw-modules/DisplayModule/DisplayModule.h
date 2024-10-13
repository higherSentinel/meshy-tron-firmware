// parent class
#ifndef DISPLAY_MODULE_H
#define DISPLAY_MODULE_H

// local includes
#include "Arduino.h"
#include "../SWModule/SWModule.h"
#include "../../generators/LinearRampFF/LinearRampFF.h"
#include "DisplayFE.h"

// local defines
#define MAX_NO_OF_DISPLAY_MODULES 4
#define MIN_PULSE_TIME_MS 0x200  // due to FF ramp
#define DISPLAY_MODULE_DEFAULT_PERIOD_MS 3072   

class DisplayModule final : SWModule
{
    public:
        ~DisplayModule() = default;
        static DisplayModule& getInstance();
        uint8_t initModule();
        void setPulsePeriod(uint32_t p);
        void run(void * params);
        static void startTsk(void * params);
        void addDisplay(DisplayFE * display);
    
    private:
        DisplayModule() = default;
        uint32_t _frame_time;
        bool _initialized;
        DisplayFE* _displays[MAX_NO_OF_DISPLAY_MODULES];
        uint8_t _dcount;
        uint32_t _pulse_period_ms = DISPLAY_MODULE_DEFAULT_PERIOD_MS;
};

#endif