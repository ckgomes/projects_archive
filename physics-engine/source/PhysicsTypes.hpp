#ifndef PHYSICS_TYPES_HPP
#define PHYSICS_TYPES_HPP

#include "Configuration.hpp"
#include "Vector2.hpp"

namespace PHYSICS_NAMESPACE
{

struct material_t
{
    f32 restitution;  // Represents the ellasticity of an object
    f32 static_coef;  // Static friction coefficient is used to determine friction when resting
    f32 dynamic_coef; // Dynamic friction coefficient is used to determine friction when in motion
};

struct body_t
{
    f32 mass;
    f32 i_mass;           // The inverse of the mass
    f32 moment_inertia;   // Moment of inertia is the difficulty to rotate an object
    f32 i_moment_inertia; // The inverse moment of inertia
};

struct transform_t
{
    vec2 position;   // Translation in world space
    f32 orientation; // Rotation in world space
    f32 scale;       // Scale in world space (TODO: actually use it)
};

struct motion_t
{
    vec2 velocity; // Linear velocity
    vec2 force;    // Linear force
    f32 omega;     // Angular velocity
    f32 torque;    // Angular force
};

struct object_t
{
    u8 type;               // Circle or Polygon -- used to determine which collision routine to use
    body_t body;           // Mass and moment of inertia
    motion_t motion;       // Linear velocity, force, angular velocity and torque
    material_t material;   // Ellasticity, friction coefficients
    transform_t transform; // Placement in world space
};

struct circle_t : public object_t
{
    f32 radius;
};

struct polygon_t : public object_t
{
    vec2* positions; // The positions of each vertex along the hull
    vec2* normals;   // The normals of each face along the hull
    u32 vertices_c;  // The number of elements in the positions and normals vector
};

struct manifold_t
{
    object_t* a;
    object_t* b;
    vec2 normal; // The collision normal represents the direction in which the least amount of penetration occurred
    f32 penetration; // The penetration value indicates by how much the two bodies are colliding
    u32 contacts_c; // Indicates how many contact points there are between the objects
    vec2 contacts[2]; // The points where the two objects are colliding
};

}

#endif // PHYSICS_TYPES_HPP
