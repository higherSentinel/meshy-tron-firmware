#ifndef TIME_SYNCHRO_H
#define TIME_SYNCHRO_H

// local incs
#include "Arduino.h"
#include "../SWModule.h"

class TimeSynchronizer final : SWModule
{
    public:
        ~TimeSynchronizer() = default;
        static TimeSynchronizer& getInstance();
        uint8_t initModule();
        void run(void * params);
        static void startTsk(void * params);
    
    private:
        TimeSynchronizer() = default;
        bool _initialized;
};


#endif