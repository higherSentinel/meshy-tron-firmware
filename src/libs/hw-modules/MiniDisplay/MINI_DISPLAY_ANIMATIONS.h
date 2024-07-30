#ifndef MINI_DISPLAY_ANIMATIONS_H
#define MINI_DISPLAY_ANIMATIONS_H

#define MAX_DIGITS_PER_FRAME        4
#define MAX_FRAMES_PER_ANIMATION    8

typedef struct
{
    char frames[MAX_FRAMES_PER_ANIMATION][MAX_DIGITS_PER_FRAME+1];
    uint8_t n_frames;
}mini_display_animation_t;

// 1
static mini_display_animation_t animation_loading_star = 
{
    {
        "   1", 
        "   /", 
        "   -", 
        "   \\"
    }, 
    4
};

// 2
static mini_display_animation_t animation_knight_rider_dot = 
{
    {
        ".   ",
        " .  ",
        "  . ",
        "   .",
        "   .",
        "  . ",
        " .  ",
        ".   ",
    }, 
    8
};

// 3
static mini_display_animation_t animation_knight_rider_dash = 
{
    {
        "-   ",
        " -  ",
        "  - ",
        "   -",
        "   -",
        "  - ",
        " -  ",
        "-   ",
    }, 
    8
};

// 4
static mini_display_animation_t animation_fwrdbkwrd = 
{
    {
        ")   ",
        " )  ",
        "  ) ",
        "   )",
        "   (",
        "  ( ",
        " (  ",
        "(   ",
    }, 
    8
};

// 5
static mini_display_animation_t animation_wave = 
{
    {
        "/   ",
        " \\  ",
        "  / ",
        "   \\",
    }, 
    4
};

// 6
static mini_display_animation_t animation_cart_wheel = 
{
    {
        "*   ",
        " *  ",
        "  * ",
        "   *",
        "  * ",
        " *  ",
    }, 
    6
};

// 7
static mini_display_animation_t animation_rolling_plank = 
{
    {
        "1   ",
        " /  ",
        "  - ",
        "   \\",
        "   1",
        "  \\ ",
        " -  ",
        "/   ",
    }, 
    8
};
#endif