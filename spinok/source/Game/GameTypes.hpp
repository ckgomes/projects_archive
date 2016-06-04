#ifndef GAME_TYPES_HPP
#define GAME_TYPES_HPP

#include "../Common/Configuration.hpp"

struct GameEntities
{
    enum Type : u16
    {
        SOLID_BLOCK = 0b00000001,
        LEDGE_BLOCK = 0b00000010,
        ITEM_BLOCK  = 0b00000100,
        PLAYER      = 0b00001000,
        THROWER     = 0b00010000,
        ITEM        = 0b00100000
    };
};

#endif // GAME_TYPES_HPP

