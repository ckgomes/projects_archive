#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "Configuration.hpp"
#include "PhysicsTypes.hpp"
#include "Mesh.hpp"
#include "Matrix2.hpp"
#include "Vector2.hpp"
#include "Math.hpp"

#include <vector>

namespace PHYSICS_NAMESPACE
{

class Physics2D final
{

    // Prevent object slicing
    // Unions can be used here because all objects are trivially constructible
    union shape_t
    {
        object_t o;
        circle_t c;
        polygon_t p;
    };

    const f32 m_timestep;
    const f32 m_half_timestep;

    const std::size_t m_max_objects;
    vec2 m_gravity;

    std::vector<shape_t> m_dynamic;
    std::vector<shape_t> m_static;

public:

    using const_object_callback_t = void(*)(const object_t&);
    using object_callback_t = void(*)(object_t&);

    Physics2D() = delete;
    explicit Physics2D(std::size_t max_objects, f32 timestep = 0.01F);

    circle_t* add(const transform_t&, const material_t&, const motion_t&, f32 density, f32 radius);
    polygon_t* add(const transform_t&, const material_t&, const motion_t&, f32 density, vec2* positions, vec2* normals, u32 vertices_c);

    void simulate();

    f32 interval() const;
    u32 entities() const;
    u32 capacity() const;
    
    vec2& gravity();

    void for_each_object(object_callback_t callback);
    void for_each_object(const_object_callback_t callback) const;

};

}

#endif // PHYSICS_HPP
