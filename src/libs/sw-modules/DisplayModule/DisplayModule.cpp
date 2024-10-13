#include "DisplayModule.h"

/**
 * singleton
*/
DisplayModule& DisplayModule::getInstance()
{
    static DisplayModule instance;
    return instance;
}

/**
 * @brief thread call, calls updates of the displays part of the module
*/
void DisplayModule::run(void * params)
{
    while(!0)
    {
        // advance the ramp
        LinearRampFF::advRamp();

        // call updates
        for(uint8_t i = 0; i < _dcount; i++)
        {
            _displays[i]->update();
        }

        // delay according to frametime
        vTaskDelay(_task_wait_time);
    }
}

/**
 * @brief init module
*/
uint8_t DisplayModule::initModule()
{
    // cal and set the pulse period
    uint32_t temp = _pulse_period_ms < MIN_PULSE_TIME_MS? MIN_PULSE_TIME_MS : _pulse_period_ms;
    _task_wait_time = temp / (double)MIN_PULSE_TIME_MS;

    // try and start the thread
    if (xTaskCreate(DisplayModule::startTsk, TASK_DISPLAY_MODULE_NAME, TASK_DISPLAY_MODULE_STACK_SIZE, NULL, TASK_DISPLAY_MODULE_PRIORITY, DisplayModule::getInstance().tsk_handle) != pdPASS)
    {
        Logger::error("DISPLAY MODULE: ", "TASK START FAIL");
        return 0xFF;
    }

    _initialized = true;
    return 0;
}

/**
 * @brief wrapper for the run method
*/
void DisplayModule::startTsk(void* params)
{
    DisplayModule::getInstance().run(params);
}

/**
 * @brief call to set the display module period
 * @param p period in ms
*/
void DisplayModule::setPulsePeriod(uint32_t p)
{
    _pulse_period_ms = p;
}

/**
 * @brief function call to add a display to the display module
 * @param display pointer to the display
*/
void DisplayModule::addDisplay(DisplayFE * display)
{
    if(display == nullptr)
        return;

    if(_dcount < MAX_NO_OF_DISPLAY_MODULES)
    {
        _displays[_dcount++] = display;
    }
}