#ifndef TRANSFORM2D_COMPONENT_HPP
#define TRANSFORM2D_COMPONENT_HPP

#include "../ECS/Component.hpp"

#include "../Common/Vector2.hpp"

struct Transform2DComponent final : public Component
{

    vec2 position;
    f32 orientation;
    vec2 scale;

    Transform2DComponent(Entity* parent);
    Transform2DComponent(Entity* parent, vec2 position);
    Transform2DComponent(Entity* parent, vec2 position, f32 orientation);
    Transform2DComponent(Entity* parent, vec2 position, f32 orientation, f32 scale);
    Transform2DComponent(Entity* parent, vec2 position, f32 orientation, vec2 scale);

    void update() override;

};

#endif // TRANSFORM2D_COMPONENT_HPP
