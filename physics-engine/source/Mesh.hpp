#ifndef MESH_HPP
#define MESH_HPP

#include "Configuration.hpp"

#include "Math.hpp"
#include "Vector2.hpp"
#include "Matrix2.hpp"

#include <vector>

namespace GRAPHICS_NAMESPACE
{

class Mesh
{

    std::vector<vec2> m_positions;
    std::vector<vec2> m_normals;

    void make_counter_clockwise();
    void center_in_COM();
    void compute_normals();

public:

    Mesh(std::vector<vec2>&& positions);

    const std::vector<vec2>& positions() const;
    std::vector<vec2>& positions();

    const std::vector<vec2>& normals() const;
    std::vector<vec2>& normals();

    u32 vertices() const;

};

}

#endif // MESH_HPP
