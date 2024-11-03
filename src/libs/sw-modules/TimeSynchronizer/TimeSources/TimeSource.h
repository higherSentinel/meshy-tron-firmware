#ifndef TIME_SRC_H
#define TIME_SRC_H

// local incs
#include <time.h>

// interface class for time source objects

class TimeSource
{
    public:
        // this function will be called to make the TS sync time from it's own source i.e. GNSS or NTP. if this function returns true the time source has successfully
        // synced it's own time and is ready to provide a valid epoch
        virtual bool syncTime()= 0; // force the children... hahaha >)

        // fucn to get epoch
        virtual time_t getEpoch()=0;

        // init
        virtual bool init();
};

#endif