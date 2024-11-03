#include "TimeSynchronizer.h"

/**
 * singleton
*/
TimeSynchronizer& TimeSynchronizer::getInstance()
{
    static TimeSynchronizer instance;
    return instance;
}

/**
 * @brief thread call
*/
void TimeSynchronizer::run(void * params)
{
    // @todo spagetti as fuck. pls fix, tks XOXO
    while(!0)
    {
        // reset back to default
        _task_wait_time_ms = TIME_SYNCHRONIZER_TSK_WAIT_TIME_MS;

        // flag for anyone who wants to use the i2c bus
        _sync_in_progress = true; 
        
        // check if secondary time source is av~ailable
        if(_sts != nullptr)
        {
            if(_sts->syncTime())
            {
                // check if we need to sync time
                if(DS3231::getInstance().getEpoch() != _sts->getEpoch())
                {
                    // set the epoch
                    DS3231::getInstance().setEpoch(_sts->getEpoch());

                    // decrease next sync time
                    _sleep_time_ms = _sleep_time_ms - TIME_SYNCHRONIZER_TIME_TO_NEXT_SYNC_DELTA_MS < 0? TIME_SYNCHRONIZER_TIME_TO_NEXT_SYNC_MINIMUM_MS : _sleep_time_ms - TIME_SYNCHRONIZER_TIME_TO_NEXT_SYNC_DELTA_MS;
                    sprintf(_str_buf, "SYNCED TIME FROM NTP, SLEEPING FOR %d SECONDS", (uint32_t)(_sleep_time_ms/1000.00));

                    // flag to update display
                    if(_udf != nullptr)
                    {
                        *_udf = true;
                    }
                }
                else
                {
                    // increment next time sync
                    _sleep_time_ms = _sleep_time_ms + TIME_SYNCHRONIZER_TIME_TO_NEXT_SYNC_DELTA_MS > 0xFFFFFFFF? 0xFFFFFFFF : _sleep_time_ms + TIME_SYNCHRONIZER_TIME_TO_NEXT_SYNC_DELTA_MS;
                    sprintf(_str_buf, "TIME IN SYNC, SKIPPING SYNCHRO, SLEEPING FOR %d SECONDS", (uint32_t)(_sleep_time_ms/1000.00));
                }

                Logger::verbose("TIME SYNCHRONIZER: ", _str_buf);
                
                // sleep longer till next epoch
                _task_wait_time_ms = _sleep_time_ms;
            };
            
        }

        // release flag
        _sync_in_progress = false;

        // thread sleep
        vTaskDelay(_task_wait_time_ms);
    }
}

/**
 * @brief init module
*/
uint8_t TimeSynchronizer::initModule()
{
    // try and start the thread
    if (xTaskCreate(TimeSynchronizer::startTsk, TASK_TIMSYNC_MODULE_NAME, TASK_TIMSYNC_MODULE_STACK_SIZE, NULL, TASK_TIMSYNC_MODULE_PRIORITY, TimeSynchronizer::getInstance().tsk_handle) != pdPASS)
    {
        Logger::error("TIME SYNCHRONIZER MODULE: ", "TASK START FAIL");
        return 0xFF;
    }

    _task_wait_time_ms = TIME_SYNCHRONIZER_TSK_WAIT_TIME_MS;

    _initialized = true;
    return 0;
}

/**
 * @brief wrapper for the run method
*/
void TimeSynchronizer::startTsk(void* params)
{
    TimeSynchronizer::getInstance().run(params);
}

/**
 * @brief call to set the primary time source
 * @param pts pointer to primary time source
*/
void TimeSynchronizer::setPrimaryTS(TimeSource* pts)
{
    _pts = pts;
}

/**
 * @brief call to set the secondary time source
 * @param sts pointer to secondary time source
*/
void TimeSynchronizer::setSecondaryTS(TimeSource* sts)
{
    _sts = sts;
}

/**
 * @brief call to check if the ts is currntly in the middle of a sync
 * @returns true if busy
*/
bool TimeSynchronizer::isBusy()
{
    return _sync_in_progress;
}


/**
 * @brief call to set the update display flag pointer. this will be set to true when a sync happens indicating the house keeper to update the display
 * @param udf pointer to update display flag
*/
void TimeSynchronizer::setUpdateDisplayFlag(bool* udf)
{
    if(udf == nullptr)
        return;

    _udf = udf;
}