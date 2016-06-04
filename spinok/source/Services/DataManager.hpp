#ifndef DATA_MANAGER_HPP
#define DATA_MANAGER_HPP

#include "../Common/Configuration.hpp"
#include "../Common/Math.hpp"
#include "../Common/Vector2.hpp"

#include "../Services/Input.hpp"
#include "../Services/Window.hpp"
#include "../Services/ImmediateRenderer2D.hpp"

#include <Box2D/Box2D.h>

#include <tuple>
#include <unordered_map>
#include <vector>
#include <array>

using MeshManager = std::unordered_map<u32, std::vector<vec2>>;

u32 generateID()
{
    static u32 id = 0;
    return ++id;
}

#endif // DATA_MANAGER_HPP
