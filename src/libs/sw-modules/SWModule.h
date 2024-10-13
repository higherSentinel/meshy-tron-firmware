#ifndef SWMODULE_H
#define SWMODULE_H

// local includes
#include "Arduino.h"
#include "../../../../task-info.h"
#include <Logger.h>

// local defines
#define MODULE_TASK_WAIT_TIME_DEFAULT_MS 1000

class SWModule
{
    public:
        TaskHandle_t * tsk_handle;
        virtual uint8_t initModule() = 0;
        virtual void run(void * params) = 0;
    
    protected:
        uint32_t _task_wait_time = MODULE_TASK_WAIT_TIME_DEFAULT_MS;
};

#endif