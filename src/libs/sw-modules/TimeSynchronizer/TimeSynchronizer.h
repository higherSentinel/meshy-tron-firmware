#ifndef TIME_SYNCHRO_H
#define TIME_SYNCHRO_H

// local incs
#include "Arduino.h"
#include <Wire.h>
#include "../SWModule.h"
#include "TimeSources/TimeSource.h"
#include "../../dev/DS3231/DS3231.h"

// local defs
#define TIME_SYNCHRONIZER_TIME_TO_NEXT_SYNC_MINIMUM_SECONDS     1
#define TIME_SYNCHRONIZER_TIME_TO_NEXT_SYNC_DELTA_MINUTES       30
#define TIME_SYNCHRONIZER_STRBUF_SIZE                           0xFF
#define TIME_SYNCHRONIZER_TSK_WAIT_TIME_MS                      10

// derived defines
#define TIME_SYNCHRONIZER_TIME_TO_NEXT_SYNC_MINIMUM_MS          (TIME_SYNCHRONIZER_TIME_TO_NEXT_SYNC_MINIMUM_SECONDS*1000)
#define TIME_SYNCHRONIZER_TIME_TO_NEXT_SYNC_DELTA_MS            (TIME_SYNCHRONIZER_TIME_TO_NEXT_SYNC_DELTA_MINUTES*60*1000)

class TimeSynchronizer final : SWModule
{
    public:
        ~TimeSynchronizer() = default;
        static TimeSynchronizer& getInstance();
        uint8_t initModule();
        void run(void * params);
        static void startTsk(void * params);
        void setPrimaryTS(TimeSource* pts);
        void setSecondaryTS(TimeSource* sts);
        bool isBusy();
    
    private:
        TimeSynchronizer() = default;
        TimeSource* _pts;
        TimeSource* _sts;
        char _str_buf[TIME_SYNCHRONIZER_STRBUF_SIZE];
        double _sleep_time_ms;
        bool _initialized;
        volatile bool _sync_in_progress = false;
};

#endif