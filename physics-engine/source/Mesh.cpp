#include "Mesh.hpp"

namespace GRAPHICS_NAMESPACE
{

void Mesh::make_counter_clockwise()
{

    // Polygon is in clockwise winding if the area is positive
    f32 mass = 0.0F;

    for (std::size_t i = 0, j = m_positions.size() - 1; i < m_positions.size(); j = i++)
        mass += (m_positions[i].x - m_positions[j].x) * (m_positions[i].y + m_positions[j].y);

    if (mass > 0.0F)
    {
        for (std::size_t i = 0, j = m_positions.size() - 1; i < j; ++i, --j)
            std::swap(m_positions[i], m_positions[j]);
    }
}

void Mesh::center_in_COM()
{
    vec2 com = {0.0F, 0.0F};
    for (auto v : m_positions) com += v;
    com /= f32(m_positions.size());
    for (auto& v : m_positions) v -= com;
}

void Mesh::compute_normals()
{
    m_normals.reserve(m_positions.size());
    for (std::size_t i = 0, j = 1; j < m_positions.size(); i = j++)
    {
        vec2 ab = m_positions[j] - m_positions[i];
        m_normals.emplace_back(ab.rotateCW90().normalize());
    }
    vec2 ab = m_positions.front() - m_positions.back();
    m_normals.emplace_back(ab.rotateCW90().normalize());
}

Mesh::Mesh(std::vector<vec2>&& positions)
    : m_positions {std::forward<std::vector<vec2>>(positions)}
{
    make_counter_clockwise();
    center_in_COM();
    compute_normals();
}

const std::vector<vec2>& Mesh::positions() const
{
    return m_positions;
}

std::vector<vec2>& Mesh::positions()
{
    return m_positions;
}

const std::vector<vec2>& Mesh::normals() const
{
    return m_normals;
}

std::vector<vec2>& Mesh::normals()
{
    return m_normals;
}

u32 Mesh::vertices() const
{
    return m_positions.size();
}



}
