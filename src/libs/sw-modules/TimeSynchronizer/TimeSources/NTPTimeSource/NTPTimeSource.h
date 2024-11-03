#ifndef NTP_TS_H
#define NTP_TS_H

// local incs
#include "../TimeSource.h"
#include "../../../../SNTPClient/SNTPClient.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include "../../../../../creds.h"
#include "../../TimeSynchronizer.h"

// local defs
#define NTPTS_WIFI_CONN_TIMEOUT_MS    30000 // 30s timeout

class NTPTimeSource : TimeSource
{   
    typedef enum
    {
        ntp_sync_sm_init,
        ntp_sync_sm_check_wifi,
        ntp_sync_sm_sntpv4,
        ntp_sync_sm_disconnect_wifi,
        ntp_sync_sm_reset,
    }ntp_sync_sm_t;

    public:
        static NTPTimeSource& getInstance();
        ~NTPTimeSource() = default;
        bool syncTime();
        time_t getEpoch();
        bool init();
        void setTimeOffset(uint32_t offset);

    private:
        NTPTimeSource() = default;
        bool _initialized = false;
        ntp_sync_sm_t _sm_state = ntp_sync_sm_init;
        WiFiUDP * _udp_ins;
        uint32_t wifi_conn_to_ts = 0;
        bool ntp_req_status = false;
};

#endif