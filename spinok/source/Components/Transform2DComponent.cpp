#include "Transform2DComponent.hpp"

Transform2DComponent::Transform2DComponent(Entity* parent)
    : Transform2DComponent(parent, {0.0F, 0.0F}, 0.0F, {1.0F, 1.0F})
{}

Transform2DComponent::Transform2DComponent(Entity* parent, vec2 position)
    : Transform2DComponent(parent, position, 0.0F, {1.0F, 1.0F})
{}

Transform2DComponent::Transform2DComponent(Entity* parent, vec2 position, f32 orientation)
    : Transform2DComponent(parent, position, orientation, {1.0F, 1.0F})
{}

Transform2DComponent::Transform2DComponent(Entity * parent, vec2 position, f32 orientation, f32 scale)
    : Transform2DComponent(parent, position, orientation, {scale, scale})
{}

Transform2DComponent::Transform2DComponent(Entity* parent, vec2 position, f32 orientation, vec2 scale)
    : Component(parent), position(position), orientation(orientation), scale(scale)
{}

void Transform2DComponent::update()
{}
