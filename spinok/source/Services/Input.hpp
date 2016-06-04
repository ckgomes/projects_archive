#ifndef INPUT_HPP
#define INPUT_HPP

#include "../Common/Configuration.hpp"
#include "../Common/LanguageExtensions.hpp"

struct Mouse
{
    bool button[16];
    f32 x, y;
    f32 dx, dy;
};

struct Keyboard
{
    bool keys[256];
    bool change[256];
    bool operator[](u8 key);
    bool isDown(u8 key);
    bool isReleased(u8 key);
};

#endif // INPUT_HPP
