#include "Map.hpp"

#include "../Common/LanguageExtensions.hpp"

#include <SDL2/SDL_image.h>
#include <cassert>

#include <iostream>

Map::Map(const char* path, b2World* world)
{
    auto image = IMG_Load(path);
    assert(image != nullptr);
    defer(SDL_FreeSurface(image));
    //assert(image->format->BytesPerPixel == 3);

    u8 pixelSize = image->format->BytesPerPixel;
    byte* pixels = static_cast<byte*>(image->pixels);

    for (s32 y = 0; y < image->h; ++y)
    {
        for (s32 x = 0; x < image->w; ++x)
        {
            s32 index = (image->h - y - 1) * image->pitch + x * pixelSize;
            u8 r = pixels[index]; u8 g = pixels[index + 1]; u8 b = pixels[index + 2];

            vec2 xy = {f32(x), f32(y)};

            if (r + g + b == 255 + 255 + 255)
            {
                m_player_spawn = xy + vec2 {0.5F, 0.5F};
            }
            else if (r == 255)
            {
                s32 end = evaluatePlatform(x, image->w, index, 0, pixelSize, pixels);
                Platform box =
                {
                    GameEntities::SOLID_BLOCK,
                    vec2 {f32(x), xy.y},
                    vec2 {f32(end), xy.y + 1.0F}
                };
                m_platforms.emplace_back(box);
                x = end - 1;
            }
            else if (g == 255)
            {
                s32 end = evaluatePlatform(x, image->w, index, 1, pixelSize, pixels);
                Platform box =
                {
                    GameEntities::LEDGE_BLOCK,
                    vec2 {f32(x), xy.y + 1.0F},
                    vec2 {f32(end), xy.y + 1.0F}
                };
                m_platforms.emplace_back(box);
                x = end - 1;
            }
            else if (b == 255)
            {
                Platform box =
                {
                    GameEntities::ITEM_BLOCK,
                    vec2 {f32(x), xy.y},
                    vec2 {f32(x + 1), xy.y + 1.0F}
                };
                m_platforms.emplace_back(box);
            }
        }
    }

    m_bodies.reserve(m_platforms.size());

    for (auto& platform : m_platforms)
    {
        vec2 mean = (platform.min + platform.max) * 0.5F;
        vec2 half = (platform.max - platform.min) * 0.5F;

        b2BodyDef bdef;
        bdef.type = b2_staticBody;
        bdef.position = {mean.x, mean.y};

        b2PolygonShape poly;
        poly.SetAsBox(half.x, half.y);

        b2FixtureDef fdef;
        fdef.restitution = 0.0F;
        fdef.friction = 1.0F;
        fdef.shape = &poly;
        fdef.userData = &platform.type;
        fdef.filter.categoryBits = platform.type;
        fdef.filter.maskBits = GameEntities::PLAYER | GameEntities::ITEM | GameEntities::THROWER;

        b2Body* body = world->CreateBody(&bdef);
        body->CreateFixture(&fdef);

        m_bodies.emplace_back(body);
    }

}

Map::~Map()
{
    b2World* world = m_bodies.front()->GetWorld();
    for (b2Body* body : m_bodies)
    {
        world->DestroyBody(body);
    }
}

vec2 Map::playerSpawn() const
{
    return m_player_spawn;
}

std::vector<Map::Platform>::iterator Map::begin()
{
    return m_platforms.begin();
}

std::vector<Map::Platform>::const_iterator Map::begin() const
{
    return m_platforms.cbegin();
}

std::vector<Map::Platform>::iterator Map::end()
{
    return m_platforms.end();
}

std::vector<Map::Platform>::const_iterator Map::end() const
{
    return m_platforms.cend();
}

s32 Map::evaluatePlatform(s32 start, s32 width, s32 index, s32 offset, u8 pixelSize, const byte* pixels)
{
    while (start < width)
    {
        index += pixelSize;
        start += 1;
        if (pixels[index + offset] != 255) break;
    }
    return start;
}
