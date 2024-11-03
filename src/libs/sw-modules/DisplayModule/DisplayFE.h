#ifndef DISPLAY_FE_H
#define DISPLAY_FE_H

// interface class for display front ends

class DisplayFE
{
    public:
        virtual void update()= 0; // force the children... hahaha >)
};

#endif