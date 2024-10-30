#include "MiniDisplay.h"

/**
 * @brief  init the nixie array
 * @param dev hpdl1414 instance
 * @warning make sure hpdl is initialized before passing the balls here 
 */
void MiniDisplay::init(HPDL1414 * dev)
{
    // sec chk
    if(dev == nullptr)
        return;

    // copy the ref over
    _dev = dev;

    // add self to display module
    DisplayModule::getInstance().addDisplay(this);

    _initialized = true;
}

/**
 * @brief set an animation
 * @param anim animation, refer to mini disp animations for more info
 */
void MiniDisplay::setAnimation(mini_display_animation_t * anim)
{
    // well sec check the life out of the pointer >)
    if(anim == nullptr)
        return;
    
    // make sure we have frames lol
    if(anim->n_frames == 0)
        return;

    _anim = anim;
    _disp_state = mini_display_state_anim;
}


/**
 * @brief call to set text on the minidisplay
 * @param txt 4 char string with only digits from 0-9 or ' ' space or '. '
 * @returns (u8) 0 on success
*/
uint8_t MiniDisplay::setText(char * txt)
{
    // sec chk
    if(txt == nullptr)
        return 0xFF;
    
    // if the display is currently busy return
    if(_disp_state == mini_display_state_text)
        return 0xFF;

    uint8_t temp = strlen(txt);
    // copy new txt
    for(uint8_t i = 0; i < MAX_DIGITS_PER_FRAME; i++)
    {
        _txt[i] = i < temp? txt[i] : ' ';
    }

    // update state
    _disp_state = mini_display_state_text;

    return 0;
}

/**
 * @brief call to blank / clear the display
*/
void MiniDisplay::blank()
{
    // update state
    _disp_state = mini_display_state_blank;
}

/**
 * @brief display module update call. called externally within animation loop
 */
void MiniDisplay::update()
{
    // sec chk
    if(!_initialized)
        return;

    // FPS
    if(millis() < (MINI_DISPLAY_FT_MS+_timestamp))
        return;

    switch(_disp_state)
    {
        case mini_display_state_idle:
            // do nothing
            break;

        case mini_display_state_blank:
            // blank and bugger off
            _dev->blank();
            _frame_counter = 0;
            _disp_state = mini_display_state_idle;
            break;

        case  mini_display_state_anim:
            _dev->setText(_anim->frames[_frame_counter++]);
            _frame_counter %= _anim->n_frames;
            break;

        case mini_display_state_text:
            // set txt and bugger off
            _dev->setText(_txt);
            _frame_counter = 0;
            _disp_state = mini_display_state_idle;
            break;

        default:
            _frame_counter = 0;
            _disp_state = mini_display_state_idle;
            break;
    }      

    _timestamp = millis();
}