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
bool SNTPClient::init(NetworkUDP* udp_inst, const char* server_name, uint64_t time_offset)
{
    // pointer patrol
    if(udp_inst == nullptr)
        return false;
    
    if(server_name == nullptr)
        return false;

    _udp = udp_inst;
    strncpy(_pool_name, server_name, SNTPC_SERVER_NAME_MAX_LEN);

    _time_offset = time_offset;
    
    // flag up
    _initialized = true;

    return true;
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
 * @returns (time_t) 
 */
time_t SNTPClient::getEpoch()
{
    if(!_initialized)
        return 0;
    
    // millis ovf
    if(millis() < _pkt_rx_ts)
        return 0;

    return (time_t)(_epoch + _time_offset + (_server_ts_ms + (millis()-_pkt_rx_ts)) / 1000.00);
}

/**
 * @brief call to set the time offset
 * @param offset offset in seconds
 */
void  SNTPClient::setTimeOffset(uint32_t offset)
{
    _time_offset = offset;
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

            // advance if udp begins
            if(_udp->begin(SNTPC_PORT) == 1)
            {
                _sm_state = sntpc_sm_flush_udp;
            }
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
                // failed, reset
                _sm_state = sntpc_sm_reset;
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
                // failed, reset
                _sm_state = sntpc_sm_reset;
                return false;
            }
            // timestamp
            _send_pkt_ts = millis();
            _await_to_ts = millis()+SNTP_RESP_WAIT_TIME_MS;
            _await_resp_ts = _send_pkt_ts;

            // advance
            _sm_state = sntpc_sm_await_response;
            break;

        // wait for the sm to reply
        case sntpc_sm_await_response:
            if(_udp->parsePacket() >= sizeof(ntp_packet_t))
            {
                // check if the polling rate is within spec
                _temp_ts = millis();

                // check if millis has overflowed
                if(_await_resp_ts > _temp_ts)
                {
                    // millis() has overflowed, restart
                    _sm_state = sntpc_sm_reset; 
                    return false;
                }

                // this check is to make sure that the caller is polling the reqNTPtime function within a speced maximum, too late of a call causes a massive
                // error in synchronization
                if(_temp_ts-_await_resp_ts > SNTP_RESP_POLL_DELTA_MS)
                {
                    _sm_state = sntpc_sm_reset; 
                    return false;
                }

                // packet went through and we got a response
                _sm_state = sntpc_sm_read_response;
                _pkt_rx_ts = _temp_ts + (0.5*(_temp_ts-_await_resp_ts));
                return false;
            }
 
            // timeout check, account for ovf
            _temp_ts = millis();
            if(_temp_ts < _send_pkt_ts || ((_temp_ts > _send_pkt_ts) && (_await_to_ts > _temp_ts)))
            {
                // waited too long
                _sm_state = sntpc_sm_reset; 
            }

            // take a time stamp of when the last call was made to parse packet. this will be used in the next call
            _await_resp_ts = millis();
            break;
        
        // read the response in to the in buf
        case sntpc_sm_read_response:
            if(_udp->read(_data_in.raw, sizeof(ntp_packet_t)) < sizeof(ntp_packet_t))
            {
                // reset
                _sm_state = sntpc_sm_reset;
                return false;
            }
            _sm_state = sntpc_sm_decode; 
            break;

        // decode the data
        case sntpc_sm_decode:
            _server_ts_ms = (ntohl(_data_in.ntp_pkt.tx_ts_f) / (double)0xFFFFFFFF) * 1024;
            _epoch = ntohl(_data_in.ntp_pkt.tx_ts_s) - 2208988800UL;
            // reset and return good news
            _sm_state = sntpc_sm_reset;
            return true;
        
        // reset
        case sntpc_sm_reset:
            _udp->stop();
            _sm_state = sntpc_sm_init;
            break;

        default:
            // reset sm
            _sm_state = sntpc_sm_reset;
            break;
    }
    return false;
}
