#include "Entity.hpp"

void Entity::scheduleRemoval()
{
    m_remove = true;
}

void Entity::update()
{
    ext::for_each(m_components, [](auto* ptr)
    {
        if (ptr != nullptr && ptr->active)
            ptr->update();
    });
}
