#include "HullComponent.hpp"

#include "../ECS/Entity.hpp"
#include "../Services/DataManager.hpp"

HullComponent::HullComponent(Entity* parent, const MeshManager* manager, u32 meshid)
    : Component (parent), m_manager(*manager), m_meshid(meshid)
{
    assert(m_manager.find(m_meshid) != m_manager.end());
}

void HullComponent::update()
{}

u32 HullComponent::getMeshID() const
{
    return m_meshid;
}
