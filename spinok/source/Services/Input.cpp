#include "Input.hpp"

bool Keyboard::operator[](u8 key)
{
    return keys[key];
}

bool Keyboard::isDown(u8 key)
{
    return keys[key] && change[key];
}

bool Keyboard::isReleased(u8 key)
{
    return !keys[key] && change[key];
}
