#include "Component.hpp"
#include "Entity.hpp"

Component::Component(Entity* parent)
    : p(parent), active(true)
{}

bool Component::isActive() const
{
    return false;
}

void Component::setActive(bool active)
{
    active = active;
}