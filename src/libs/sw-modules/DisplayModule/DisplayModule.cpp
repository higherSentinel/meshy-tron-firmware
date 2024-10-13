#include "DisplayModule.h"

TaskHandle_t* DisplayModule::dispmod_tsk_handle;
DisplayFE* DisplayModule::_displays[MAX_NO_OF_DISPLAY_MODULES];
uint8_t DisplayModule::_dcount;
uint32_t DisplayModule::_frame_time;
bool DisplayModule::_initialized;

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
        vTaskDelay(_frame_time);
    }
}

/**
 * @brief thread call, calls "updates" of the displays part of the module
 * @param 
*/
uint8_t DisplayModule::initModule(uint32_t pulse_period_ms)
{
    // cal and set the pulse period
    uint32_t temp = pulse_period_ms < MIN_PULSE_TIME_MS? MIN_PULSE_TIME_MS : pulse_period_ms;
    _frame_time = temp / (double)MIN_PULSE_TIME_MS;

    // try and start the thread
    if (xTaskCreate(DisplayModule::run, TASK_DISPLAY_MODULE_NAME, TASK_DISPLAY_MODULE_STACK_SIZE, NULL, TASK_DISPLAY_MODULE_PRIORITY, DisplayModule::dispmod_tsk_handle) != pdPASS)
    {
        Logger::error("DISPLAY MODULE: ", "TASK START FAIL");
        return 0xFF;
    }

    _initialized = true;
    return 0;
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