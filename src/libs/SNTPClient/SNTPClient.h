#ifndef SNTP_CLIENT_H
#define SNTP_CLIENT_H

// local includes
#include "Arduino.h"
#include "NetworkUdp.h"


// local defines
#define SNTPC_DEFAULT_SERVER        "fi.pool.ntp.org"
#define SNTPC_PORT                  123
#define SNTPC_SERVER_NAME_MAX_LEN   0x40
#define SNTP_RESP_WAIT_TIME_MS      1000
#define SNTP_RESP_POLL_DELTA_MS     50                 

// not to be used with parallel udp streams, call to getNTPtime flushes the udp buffers. use with caution

class SNTPClient
{
    // ntp packet stuct RFC5905: https://www.ietf.org/rfc/rfc5905.txt
    #pragma pack(push,1)
    typedef struct
    {
        uint8_t mode:3;               // mode
        uint8_t vn:3;                 // version number
        uint8_t li:2;                 // leep indicator

        uint8_t stratum;              // stratum level of the local clock
        int8_t poll;                  // maximum interval between successive messages
        int8_t precision;             // precision of the local clock

        uint32_t root_delay;          // total round trip delay time
        uint32_t root_dispersion;     // max error aloud from primary clock source
        uint32_t ref_Id;              // reference clock identifier

        uint32_t ref_ts_s;            // reference time-stamp seconds
        uint32_t ref_ts__f;           // reference time-stamp fraction of a second

        uint32_t orig_ts_s;           // originate time-stamp seconds
        uint32_t orig_ts_f;           // originate time-stamp fraction of a second

        uint32_t rx_ts_s;             // received time-stamp seconds
        uint32_t rx_ts_f;             // received time-stamp fraction of a second

        uint32_t tx_ts_s;             // transmit time-stamp seconds
        uint32_t tx_ts_f;             // transmit time-stamp fraction of a second

    } ntp_packet_t;
    #pragma pack(pop)

    typedef union
    {
    ntp_packet_t ntp_pkt;
    uint8_t raw[sizeof(ntp_packet_t)];
    }ntp_stream_t;

    typedef enum
    {
        sntpc_sm_init,
        sntpc_sm_flush_udp,
        sntpc_sm_begin_pkt,
        sntpc_sm_send_packet,
        sntpc_sm_await_response,
        sntpc_sm_read_response,
        sntpc_sm_decode,
        sntpc_sm_reset,
    }sntpc_sm_t;

    typedef union
    {
        uint32_t i32u[2];
        uint64_t i64u;
    }i32u2i64u_t;


    public:
        static SNTPClient& getInstance();
        ~SNTPClient();
        bool reqNTPtime();
        void init(NetworkUDP* udp_inst, const char* server_name = SNTPC_DEFAULT_SERVER, uint64_t time_offset = 0);
        void setServerName(const char* name);
        uint32_t getEpoch();
        void setServerIP(IPAddress ip);


    private:
        SNTPClient();
        ntp_stream_t    _data_out; 
        ntp_stream_t    _data_in; 
        NetworkUDP*     _udp;
        char            _pool_name[SNTPC_SERVER_NAME_MAX_LEN];
        IPAddress       _pool_ip;
        bool            _initialized;
        bool            _use_ip = false;
        sntpc_sm_t      _sm_state;
        uint16_t        _fcount;
        uint32_t        _epoch;
        uint32_t        _temp_ts;
        uint32_t        _send_pkt_ts;
        uint32_t        _await_to_ts;
        uint32_t        _await_resp_ts;
        uint32_t        _pkt_rx_ts;
        uint32_t        _server_ts_ms;
};

#endif