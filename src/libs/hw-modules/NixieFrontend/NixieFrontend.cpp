#include "NixieFrontend.h"

/**
 * @brief  init the nixie array
 * @param devs struct with the pointers to the digit devices and the latch pin
 */
void NixieFrontend::init(nixie_frontend_t * devs)
{
    // sec chk
    if(devs == nullptr)
        return;
    // sec chk more
    for(uint8_t i = 0; i < MAX_NUMBER_OF_DIGITS; i++)
    {
        if(devs->nixies[i] == nullptr)
            return;
    }

    // copy the ref over
    _devs = devs;

    // turn off everything and set the nixies to zero
    for(uint8_t i = 0; i < MAX_NUMBER_OF_DIGITS; i++)
    {
        _nixie_digits[i] = convertToNixieDigit(' ');
        _devs->nixies[i]->setDigit(_nixie_digits[i]);
        _devs->nixies[i]->setBrightness(0);
        _nixie_ustate[i] = nixie_update_idle;
    }
    latchNixies();

    // add self to display module
    DisplayModule::addDisplay(this);

    _initialized = true;
}

/**
 * @brief set fade type
 * @param ft fade type, refer to class header
 */
void NixieFrontend::setFadeType(nixie_fade_animation_t ft)
{
    _ft = ft;
}


/**
 * @brief call to set the hour digits
 * @param txt 4 char string with only digits from 0-9 or ' ' space or '. '
*/
void NixieFrontend::setText(char * txt)
{
    // sanity chk
    if(!_initialized || txt == nullptr || _update)
        return;

    // more sanity check
    if(strlen(txt) < MAX_NUMBER_OF_DIGITS)
        return;

    // few more for bureaucracy
    for(uint8_t i = 0; i < MAX_NUMBER_OF_DIGITS; i++)
    {
        if(convertToNixieDigit(txt[i]) == 0xFF)
            return;
    }

    // if instant set and f'off
    if(_ft == nixie_fade_animation_instant)
    {
        for(uint8_t i = 0; i < MAX_NUMBER_OF_DIGITS; i++)
        {
            _nixie_digits[i] = convertToNixieDigit(txt[i]);
            _devs->nixies[i]->setDigit(_nixie_digits[i]);
            _devs->nixies[i]->setBrightness(txt[i] == ' '? 0 : 0xFF);
        }
        latchNixies();
        return;
    }

    // user always wants more.. sigh
    // find out which nixies need to be updated
    for(uint8_t i = 0; i < MAX_NUMBER_OF_DIGITS; i++)
    {
        // if the nixie isn't showing what was requested then update it and
        // append a sendout request, we will also need to only ramp down if the
       // requested char is a blank   
        if(_nixie_digits[i] != convertToNixieDigit(txt[i]))
        {            
            // ramp down only check
            _nixie_ustate[i] = txt[i] == ' '? nixie_update_ramp_dwn_only : nixie_update_ramp_dwn;

            // ramp up only check
            _nixie_ustate[i] = _nixie_digits[i] == convertToNixieDigit(' ')? nixie_update_ramp_up : _nixie_ustate[i];

            _nixie_digits[i] = convertToNixieDigit(txt[i]);
        }
    }
    
    // set internal flag to ensure digits are pushed out
    _update = true;
}

/**
 * @brief display module update call. called externally within animation loop
 */
void NixieFrontend::update()
{
    // sec chk
    if(!_initialized || !_update)
        return;

    // cool part here is that if i decide to add anyother generators i just 
    // switch case this :)
    uint8_t rv = LinearRampFF::getSample();
    
    // update requested
    switch(_nixie_update_loop_state)
    {
        // wait for the neg trans ---
        case nixie_ulsm_ramp_init:
            if(LinearRampFF::isGradNeg() && rv == 0xFF)
            {
                // advance
                _nixie_update_loop_state = nixie_ulsm_ramp_down;   
            }
            break;
        // ramp down the nixies which need to be ramped down ---
        case nixie_ulsm_ramp_down:
            for(uint8_t i = 0; i < MAX_NUMBER_OF_DIGITS; i++)
            {
                if(_nixie_ustate[i] > nixie_update_ramp_up)
                {
                    // rv should be on the downward here we can just assign
                    _devs->nixies[i]->setBrightness(rv);
                }
                if(rv == 0)
                {
                    // end of the ramp dwn, digits can now be bashed 
                    // to death in the dark >), gotta bash em alllll... nixiemon
                    // cries in common latch
                    _devs->nixies[i]->setDigit(_nixie_digits[i]);
                    _nixie_ustate[i] = _nixie_ustate[i] > nixie_update_ramp_up? (nixie_update_t)(_nixie_ustate[i] - 1) : _nixie_ustate[i];
                }
            }
            
            // cries again on common latch, sad lipe
            if(rv == 0)
            {
                // at this point all the nixies have thier data updated and ready to be
                // latched to the secondary register
                latchNixies();
                
                // advance
                _nixie_update_loop_state = nixie_ulsm_ramp_up;
            }
            break;
        
        // ramp up the nixies which need to be ramped up ---
        case nixie_ulsm_ramp_up:
            for(uint8_t i = 0; i < MAX_NUMBER_OF_DIGITS; i++)
            {
                if(_nixie_ustate[i] == nixie_update_ramp_up)
                {
                    // rv should be on the upward here we can just assign
                    _devs->nixies[i]->setBrightness(rv);
                }
                if(rv == 0xFF)
                {
                    // end of the ramp up, dead nixies are reborn
                    _nixie_ustate[i] = nixie_update_idle;
                }
            }

            // copium at this point
            if(rv == 0xFF)
            {
                // end of the update, release flag and reset
                _nixie_update_loop_state = nixie_ulsm_ramp_init;
                _update = false;
            }
            break;
        default:
            // this should never happen but jic
            _nixie_update_loop_state = nixie_ulsm_ramp_init;
            break;
    }
        

}

/**
 * @brief internal function to latch nixies
 */
void NixieFrontend::latchNixies()
{
    // sanity chk
    if(!_initialized)
        return;

    digitalWrite(_devs->latch_pin, HIGH);
    // delay not necessary but jic
    delayMicroseconds(1);
    digitalWrite(_devs->latch_pin, LOW);
}

/**
 * @brief function call to convert the char to nixie lingo
 * @param c char to be converted duh
 * @returns u8 nixie lingo
*/
uint8_t NixieFrontend::convertToNixieDigit(char c)
{
    if(c > 47 && c < 58)
    {
        return c-48;
    }
    else if(c == ' ')
    {
        // this is bad code to the point where i would like to kms
        return 11;
    }
    else if(c == '.')
    {
        // this is bad code to the point where i would like to kms
        return 10;
    }
    return 0xFF;
}
