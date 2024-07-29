// parent class
#ifndef DISPLAY_MODULE_H
#define DISPLAY_MODULE_H

// local includes
#include "Arduino.h"
#include "libs/generators/LinearRampFF/LinearRampFF.h"

class DisplayModule
{
    public:
        DisplayModule() = default;
        ~DisplayModule() = default;
        virtual void update()= 0; // force the children... hahaha >)
        static uint8_t initModule();
        static void run();
};

#endif