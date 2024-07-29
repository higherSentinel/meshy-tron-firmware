#ifndef NIXIE_FRONTEND_H
#define NIXIE_FRONTEND_H

// local includes
#include "Arduino.h"
#include "../../dev/SRnixie/SRnixie.h"
#include "../../../DisplayModule.h"

// local defines
#define MAX_NUMBER_OF_DIGITS 4

typedef struct
{
    SRnixie* nixies[MAX_NUMBER_OF_DIGITS];
    uint32_t latch_pin; // only for shift-register based nixies
}nixie_frontend_t;

typedef enum
{
    nixie_fade_animation_instant,
    nixie_fade_animation_linear,
}nixie_fade_animation_t;

typedef enum 
{
    nixie_ulsm_ramp_init,
    nixie_ulsm_ramp_down,
    nixie_ulsm_ramp_up,
}nixie_ulsm_t;

typedef enum
{
    nixie_update_idle = 0,
    nixie_update_ramp_up,
    nixie_update_ramp_dwn,
    nixie_update_ramp_dwn_only,
}nixie_update_t;

class NixieFrontend : DisplayModule
{
    public:
        ~NixieFrontend() = default;
        // singleton
        static NixieFrontend& getInstance()
        {
            static NixieFrontend instance;
            return instance;
        }

        void init(nixie_frontend_t * ar);
        void setFadeType(nixie_fade_animation_t ft);
        void setText(char * str);
        void setMinutes(uint8_t mins);
        void setHours(uint8_t hrs);
        void update();

    private:
        NixieFrontend() = default;
        void latchNixies();
        uint8_t convertToNixieDigit(char c);

        nixie_frontend_t*           _devs;
        bool                        _initialized = false;
        bool                        _update = false;
        uint8_t                     _nixie_digits[MAX_NUMBER_OF_DIGITS];
        nixie_update_t              _nixie_ustate[MAX_NUMBER_OF_DIGITS];
        nixie_ulsm_t                _nixie_update_loop_state = nixie_ulsm_ramp_init;
        nixie_fade_animation_t      _ft = nixie_fade_animation_instant;
}; 

#endif 