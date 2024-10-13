#ifndef MINI_DISPLAY_H
#define MINI_DISPLAY_H

// local includes
#include "Arduino.h"
#include "../../dev/HPDL1414/HPDL1414.h"
#include "./../../sw-modules/DisplayModule/DisplayModule.h"
#include "MINI_DISPLAY_ANIMATIONS.h"

// local defines
#define MINI_DISPLAY_FPS            15.00
#define MINI_DISPLAY_FT_MS          1000.00 / MINI_DISPLAY_FPS

typedef enum
{
    mini_display_state_idle,
    mini_display_state_anim,
    mini_display_state_blank,
    mini_display_state_text,
}mini_display_state_t;

class MiniDisplay : DisplayFE
{
    public:
        ~MiniDisplay() = default;
        // singleton
        static MiniDisplay& getInstance()
        {
            static MiniDisplay instance;
            return instance;
        }

        void init(HPDL1414* dev);
        void setAnimation(mini_display_animation_t * anim);
        uint8_t setText(char * str);
        void update();

    private:
        MiniDisplay() = default;
        void latchNixies();
        uint8_t convertToNixieDigit(char c);

        HPDL1414*                   _dev;
        bool                        _initialized = false;
        mini_display_state_t        _disp_state = mini_display_state_blank;
        mini_display_animation_t*   _anim = nullptr;
        char                        _txt[MAX_DIGITS_PER_FRAME];
        uint32_t                    _timestamp;
        uint8_t                     _frame_counter;
}; 

#endif 