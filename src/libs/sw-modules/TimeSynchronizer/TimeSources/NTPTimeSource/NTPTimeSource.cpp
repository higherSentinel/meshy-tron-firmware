#include "NTPTimeSource.h"

// singleton
NTPTimeSource& NTPTimeSource::getInstance()
{
    static NTPTimeSource instance;
    return instance;
}

/**
 * @brief call init
*/
bool NTPTimeSource::init()
{
    // make the udp instance
    _udp_ins = new WiFiUDP();

    // init sntpc
    if(!SNTPClient::getInstance().init(_udp_ins))
    {
        return false;
    }

    // register self as a secondary time source
    TimeSynchronizer::getInstance().setSecondaryTS(this);

    // fup
    _initialized = true;
    return true;
}

/**
 * @brief call to sync time
 * @returns true if sync time was successful, this needs to be polled frequently
*/
bool NTPTimeSource::syncTime()
{
    if(!_initialized)
        return false;

    switch(_sm_state)
    {
        // init
        case ntp_sync_sm_init:
            // begin wifi and move to connect check
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            wifi_conn_to_ts = millis();
            ntp_req_status = false;
            _sm_state = ntp_sync_sm_check_wifi; 
            break;
        
        // wifi status
        case ntp_sync_sm_check_wifi:
            if(WiFi.status() == WL_CONNECTED)
            {
                // ts & move
                 wifi_conn_to_ts = millis();
                _sm_state = ntp_sync_sm_sntpv4;
            }
            // check for millis ovf
            else if(millis() < wifi_conn_to_ts || (millis() > (wifi_conn_to_ts+NTPTS_WIFI_CONN_TIMEOUT_MS)))
            {
                // timed out, disconnect and try again
                _sm_state = ntp_sync_sm_disconnect_wifi;
            }
            break;
        
        // this will call the udp req time which is it's own state machine, it will return true if it got a response from the ntp server
        // loop back and forth between checking for wifi and ntp req
        case ntp_sync_sm_sntpv4:
            if(SNTPClient::getInstance().reqNTPtime())
            {
                // set flag and disconnect WiFi
                ntp_req_status = true;
                _sm_state = ntp_sync_sm_disconnect_wifi;
                return false;
            }
            // back to wifi check
            _sm_state = ntp_sync_sm_check_wifi;
            break;

        case ntp_sync_sm_disconnect_wifi:
            WiFi.disconnect();
            _sm_state = ntp_sync_sm_reset;
            break;

        case ntp_sync_sm_reset:
            // for now back to init
            _sm_state = ntp_sync_sm_init;
            // return status
            return ntp_req_status;
            
        default:
            // reset
            _sm_state = ntp_sync_sm_reset;
            break;
    }

    return false;
}

/**
 * @brief call to get epcoch
 * @returns (time_t) epoch in seconds
*/
time_t NTPTimeSource::getEpoch()
{
    if(!_initialized)
        return 0;
    
    return SNTPClient::getInstance().getEpoch();
}

/**
 * @brief call to set the time offset
 * @param offset offset in seconds
 */
void NTPTimeSource::setTimeOffset(uint32_t offset)
{
    SNTPClient::getInstance().setTimeOffset(offset);
}