#include "SNTPClient.h"

// singleton
SNTPClient& SNTPClient::getInstance()
{
    static SNTPClient instance;
    return instance;
}

// const and destuct
SNTPClient::SNTPClient(){}
SNTPClient::~SNTPClient(){}

/** 
 * @brief call to initialize the sntp client
 * @param udp_inst instance pointing to the UDP object. caller is responsible for creating the object
 * @param time_offset time zone diff
*/
void SNTPClient::init(NetworkUDP* udp_inst, const char* server_name, uint64_t time_offset)
{
    // pointer patrol
    if(udp_inst == nullptr)
        return;
    
    if(server_name == nullptr)
        return;

    _udp = udp_inst;
    _udp->begin(SNTPC_PORT);
    strncpy(_pool_name, server_name, SNTPC_SERVER_NAME_MAX_LEN);
    
    // flag up
    _initialized = true;
}

/**
 * @brief call to set the server name
 * @param name domain name of the ntp server
 */
void SNTPClient::setServerName(const char* name)
{
    // pointer patrol
    if(name == nullptr)
        return;

    strncpy(_pool_name, name, SNTPC_SERVER_NAME_MAX_LEN);
    _use_ip = false;
}

/**
 * @brief call to set the server IP
 * @param ip IP address of the ntp server
 */
void SNTPClient::setServerIP(IPAddress ip)
{
    _pool_ip = ip;
    _use_ip = true;
}

/**
 * @brief call to get the epoch
 * @returns (uint32_t) 
 */
uint32_t SNTPClient::getEpoch()
{
    if(!_initialized)
        return 0;
    
    return _epoch;
}


/**
 * @brief call to sync time from the ntp server, this is a state machine which can be repeatedly called. the sm initiates a udp request
 * to the server to sync time 
 * @returns true on success. It's recommended to read the epoch before the next call to getNTPTime
 */
bool SNTPClient::reqNTPtime()
{
    // to be used in sm sub states
    int ret = 0;

    switch(_sm_state)
    {
        case sntpc_sm_init:
            // reset
            memset(_data_out.raw, 0, sizeof(ntp_packet_t));

            // setup ntp v4
            _data_out.ntp_pkt.li = 3;           // unknown
            _data_out.ntp_pkt.vn = 4;           // v4
            _data_out.ntp_pkt.mode = 3;         // client
            _data_out.ntp_pkt.stratum = 0;      // unspecified
            _data_out.ntp_pkt.poll = 6;         // speced minimum
            _data_out.ntp_pkt.precision = -20;  // 2^-20 ~ 1us precision

            // setup a sick message in refID, refID is typically used for clock identification, we identify as SICK, but the server will
            // most likely respond with it's ip, on rare occasions we might(never) get GOES
            _data_out.ntp_pkt.ref_Id = 0x5349434B;

            _fcount = 0;

            // advance
            _sm_state = sntpc_sm_flush_udp;
            break;
            
        // flush out the udp buffers
        case sntpc_sm_flush_udp:
            if(_udp->parsePacket() != 0)
            {
                _udp->flush();
                return false;
            }

            // advance
            _sm_state = sntpc_sm_begin_pkt;
            break;

        // this will establish a udp stream on the speced port and name, if unsuccessfull something went wrong
        case sntpc_sm_begin_pkt:
            ret = _udp->beginPacket(_pool_name, SNTPC_PORT);
            if(ret == 0)
            {
                // failed reset to init
                _sm_state = sntpc_sm_init;
                return false;
            }

            // write data to the packet
            _udp->write(_data_out.raw, sizeof(ntp_packet_t));

            // advance if successful
            _sm_state = sntpc_sm_send_packet;
            break;

        // send the packet off
        case sntpc_sm_send_packet:
            ret = _udp->endPacket();
            if(ret == 0)
            {
                // failed reset to init
                _sm_state = sntpc_sm_init;
                return false;
            }

            // advance
            _sm_state = sntpc_sm_await_response;
            break;

        // wait for the sm to reply
        case sntpc_sm_await_response:
            if(_udp->parsePacket() >= sizeof(ntp_packet_t))
            {
                // packet went through and we got a response
                _sm_state = snptc_sm_read_response;
            }
            if(_fcount > SNTPC_FCOUNT)
            {
                // waited too long
                _sm_state = sntpc_sm_init; 
            }
            _fcount++;
            break;
        
        // read the response in to the in buf
        case snptc_sm_read_response:
            if(_udp->read(_data_in.raw, sizeof(ntp_packet_t)) < sizeof(ntp_packet_t))
            {
                // reset
                _sm_state = sntpc_sm_init;
                return false;
            }
            _sm_state = snptc_sm_decode; 
            break;

        // decode the data
        case snptc_sm_decode:
            _epoch = ntohl(_data_in.ntp_pkt.tx_ts_s) - 2208988800UL;
            // reset and return good news
            _sm_state = sntpc_sm_init;
            return true;

        default:
            // reset sm
            _sm_state = sntpc_sm_init;
            break;
    }
    return false;
}
