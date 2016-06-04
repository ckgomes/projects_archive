#ifndef COMPONENT_LIST_HPP
#define COMPONENT_LIST_HPP

#include "../Common/Configuration.hpp"
#include "../Common/LanguageExtensions.hpp"

#include "../Components/Transform2DComponent.hpp"
#include "../Components/GraphicsComponent.hpp"
#include "../Components/PhysicsComponent.hpp"
#include "../Components/HullComponent.hpp"
#include "../Components/PlayerComponent.hpp"

struct Components
{

    using ListPointer = std::tuple
    <
        Transform2DComponent*,
        PlayerComponent*,
        GraphicsComponent*,
        PhysicsComponent*,
        HullComponent*
    >;

    static const size_t count = std::tuple_size<ListPointer>::value;

};

#endif // COMPONENT_LIST_HPP
