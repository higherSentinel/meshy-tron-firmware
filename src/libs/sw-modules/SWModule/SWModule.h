#ifndef SWMODULE_H
#define SWMODULE_H

// local includes
#include "Arduino.h"
#include "../../../../task-info.h"
#include <Logger.h>

class SWModule
{
    public:
        TaskHandle_t * tsk_handle;
        virtual uint8_t initModule() = 0;
        virtual void run(void * params) = 0;
};

#endif