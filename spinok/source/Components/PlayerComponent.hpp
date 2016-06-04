#ifndef PLAYER_COMPONENT_HPP
#define PLAYER_COMPONENT_HPP

#include "../ECS/Component.hpp"

#include "../Services/Input.hpp"
#include "../Services/Timer.hpp"
#include "../Services/ImmediateRenderer2D.hpp"

class PlayerComponent : public Component
{

public:

    PlayerComponent(Entity* parent, ImmediateRenderer2D* renderer, Keyboard* keyboard);
    ~PlayerComponent();

    void update() override;

private:

    ImmediateRenderer2D& m_renderer;
    Keyboard& m_keyboard;
    Timer timer;

    size_t m_contacts;
    f64 m_jumped;
    u8 steps; // Stores for how many frames we will apply force to the player (jump)

};

#endif // PLAYER_COMPONENT_HPP
