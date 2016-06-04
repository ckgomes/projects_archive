#include "Transform2DComponent.hpp"
#include "GraphicsComponent.hpp"
#include "HullComponent.hpp"

#include "../ECS/Entity.hpp"

GraphicsComponent::GraphicsComponent(Entity* parent, ImmediateRenderer2D* renderer, const MeshManager* manager)
    : Component(parent), m_renderer(*renderer), m_manager(*manager)
{
    assert(p->hasComponent<HullComponent>());
    assert(p->hasComponent<Transform2DComponent>());
}

void GraphicsComponent::update()
{
    constexpr vec2 positionalOffset = {0.0F, -0.025F};

    auto transform = p->getComponent<Transform2DComponent>();
    auto hull = p->getComponent<HullComponent>();

    const std::vector<vec2>& mesh = m_manager.at(hull->getMeshID());

    m_renderer.setColor(0.1F, 0.1F, 0.2F);
    m_renderer.pushTransform(transform->position + positionalOffset, transform->orientation, transform->scale);
    m_renderer.drawPolygon(mesh);
    m_renderer.popTransform();
}
