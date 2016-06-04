#include <SDL2/SDL_main.h>

#include "Services/Font.hpp"
#include "Services/Input.hpp"
#include "Services/Timer.hpp"
#include "Services/Window.hpp"
#include "Services/DataManager.hpp"
#include "Services/ImmediateRenderer2D.hpp"

#include "ECS/Entity.hpp"

#include "Game/CollisionCallback.hpp"
#include "Game/Map.hpp"

#include <iostream>

static ContactListener listener;

int main(int argc, char** args)
{

    Timer timer;
    Window window("Spinok", 640, 480);
    ImmediateRenderer2D renderer(window, false);
    MeshManager manager;
    b2World physics({0.0F, -9.81F});
    Mouse mouse = {};
    Keyboard keyboard = {};

    f64 accumulator = 0.0;
    Map map("../asset/map.png", &physics);

    std::vector<vec2> triangle =
    {
        {+0.0F, +0.5F},
        {-0.5F, -0.5F},
        {+0.5F, -0.5F}
    };
    std::vector<vec2> quad =
    {
        {-0.5F, -0.5F},
        {-0.5F, +0.5F},
        {+0.5F, +0.5F},
        {+0.5F, -0.5F}
    };

    manager.emplace(0, std::move(triangle));
    manager.emplace(1, std::move(quad));

    constexpr u32 max_entities = 10;
    std::vector<Entity> entities(max_entities);
    
    entities[0].addComponent<HullComponent>(&manager, 0);
    entities[0].addComponent<Transform2DComponent>(map.playerSpawn());
    entities[0].addComponent<GraphicsComponent>(&renderer, &manager);

    entities[0].addComponent<PhysicsComponent>(&physics, &manager, GameEntities::PLAYER,
      GameEntities::SOLID_BLOCK | GameEntities::ITEM_BLOCK | GameEntities::LEDGE_BLOCK);

    entities[0].addComponent<PlayerComponent>(&renderer, &keyboard);

    physics.SetAllowSleeping(false);
    physics.SetContactListener(&listener);

    renderer.orthographic(0.0, 12.0 * window.aspectRatio(), 0.0, 12.0);
    u32 bg = renderer.loadTexture("../asset/clouds.png");   

    Font font("../asset/AdventPro-SemiBold.ttf");

    timer.reset();
    f64 last = timer.elapsed();

    while (window.update(keyboard, mouse) && !keyboard[27])
    {
        f64 elapsed = timer.elapsed();
        f64 dt = elapsed - last;
        last = elapsed;
        accumulator += dt;

        while (accumulator > 0.0)
        {
            constexpr f32 physics_dt = (1.0F / 60.0F);
            physics.Step(physics_dt, 6, 2);
            accumulator -= physics_dt;
        }

        renderer.prepare();

        for (auto& entity : entities) entity.update();

        for (auto& platform : map)
        {
            constexpr vec2 passablebelowoffset = vec2 {0.0F, -0.025F};
            switch (platform.type)
            {
            case GameEntities::SOLID_BLOCK:
                renderer.setColor(0.2F, 0.0F, 0.0F);
                renderer.drawBox(platform.min, platform.max);
                break;
            case GameEntities::LEDGE_BLOCK:
                renderer.setColor(0.0F, 0.2F, 0.0F);
                renderer.drawLine(platform.min + passablebelowoffset, platform.max + passablebelowoffset);
                break;
            case GameEntities::ITEM_BLOCK:
                renderer.setColor(0.0F, 0.0F, 0.2F);
                renderer.drawBox(platform.min, platform.max);
                break;
            }
        }

        renderer.setColor(0.0F, 0.0F, 0.0F);
        renderer.drawText(font, "FPS: " + std::to_string(renderer.getFrameRate()), {5.0F, 456.0F}, 20.0F);
        renderer.present();
    }
    
    return 0;
}