#ifndef ENTITY_COMPONENT_SYSTEM_HPP
#define ENTITY_COMPONENT_SYSTEM_HPP

#include "../Common/Configuration.hpp"
#include "../Common/LanguageExtensions.hpp"

#include "ComponentList.hpp"

#include <cassert>

class Entity final
{

public:
   
    template <typename component_t, typename... args_t>
    void addComponent(args_t... args);

    template <typename component_t>
    void setActive(bool active);

    template <typename component_t>
    component_t* getComponent();

    template <typename component_t>
    bool hasComponent() const; 

    // The entity will be removed before the next frame begins, but not immediately
    void scheduleRemoval();

    // Calls update on all components
    void update();

private:

    // Pointers must be used to prevent object slicing
    Components::ListPointer m_components = {};
    bool m_remove = false;

};

template <typename component_t, typename... args_t>
inline void Entity::addComponent(args_t... args)
{
    static_assert(ext::tuple_contains_type<component_t*, Components::ListPointer>::value, "The type is not a registred as a [Component]");
    static_assert(std::is_base_of<Component, component_t>::value, "The type does not inherit from [Component]");
    auto& ptr = std::get<component_t*>(m_components);
    assert(ptr == nullptr);
    ptr = new component_t(this, args...);
}

template <typename component_t>
inline void Entity::setActive(bool active)
{
    static_assert(ext::tuple_contains_type<component_t*, Components::ListPointer>::value, "The type is not a registred as a [Component]");
    static_assert(std::is_base_of<Component, component_t>::value, "The type does not inherit from [Component]");
    auto ptr = std::get<component_t*>(m_components);
    assert(ptr != nullptr);
    ptr->m_active = active;
}

template <typename component_t>
inline component_t* Entity::getComponent()
{
    static_assert(ext::tuple_contains_type<component_t*, Components::ListPointer>::value, "The type is not a registred as a [Component]");
    static_assert(std::is_base_of<Component, component_t>::value, "The type does not inherit from [Component]");
    auto ptr = std::get<component_t*>(m_components);
    assert(ptr != nullptr);
    return ptr;
}

template <typename component_t>
inline bool Entity::hasComponent() const
{
    static_assert(ext::tuple_contains_type<component_t*, Components::ListPointer>::value, "The type is not a registred as a [Component]");
    static_assert(std::is_base_of<Component, component_t>::value, "The type does not inherit from [Component]");
    auto ptr = std::get<component_t*>(m_components);
    return (ptr != nullptr);
}

#endif // ENTITY_COMPONENT_SYSTEM_HPP
