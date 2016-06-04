#ifndef MAP_HPP
#define MAP_HPP

#include "../Common/Configuration.hpp"
#include "../Common/Vector2.hpp"

#include "GameTypes.hpp"

#include <vector>

#include <Box2D/Box2D.h>

class Map final
{

public:

    struct Platform
    {
        GameEntities::Type type;
        vec2 min;
        vec2 max;
    };

    Map(const char* path, b2World* world);
    ~Map();

    vec2 playerSpawn() const;

    std::vector<Platform>::iterator begin();
    std::vector<Platform>::const_iterator begin() const;

    std::vector<Platform>::iterator end();
    std::vector<Platform>::const_iterator end() const;

private:

    s32 evaluatePlatform(s32 start, s32 width, s32 index, s32 offset, u8 pixelSize, const byte* pixels);

    std::vector<Platform> m_platforms;
    std::vector<b2Body*> m_bodies;
    vec2 m_player_spawn;

};

#endif // MAP_HPP

