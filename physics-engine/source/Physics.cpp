#include "Physics.hpp"

#include <iostream>
#include <algorithm>

namespace PHYSICS_NAMESPACE
{

// Object type enumeration is used to index the dispatch function in the collision vtable
enum object_type_t
{
    circle,
    polygon,
    object_type_count
};

/// Collision detections functions

static bool collides_circle_circle(manifold_t& m)
{
    assert((m.a->type) == circle);
    assert((m.b->type) == circle);

    auto a = static_cast<circle_t*>(m.a);
    auto b = static_cast<circle_t*>(m.b);

    vec2 ab = (b->transform.position) - (a->transform.position);
    f32 ab_radius = (a->radius) + (b->radius);
    f32 ab_radius_sq = ab_radius * ab_radius;

    if (ab.lengthSq() > ab_radius_sq)
        return false;

    f32 distance = ab.length();

    if (distance > 0.0F)
    {
        m.penetration = ab_radius - distance;
        m.normal = ab / distance;
    }
    else // Corner case: centers coincide
    {
        m.penetration = a->radius;
        m.normal = {0.0F, 1.0F};
    }
    return true;
}

static bool collides_circle_polygon(manifold_t& m)
{
    assert((m.a->type) == circle);
    assert((m.b->type) == polygon);
    auto a = static_cast<circle_t*>(m.a);
    auto b = static_cast<polygon_t*>(m.b);
    m.contacts_c = 0;
    vec2 center = ((a->transform.position) - (b->transform.position)).rotate(-b->transform.orientation);
    f32 separation = -math::infinity();
    u32 face_normal = 0;
    for (u32 i = 0; i < (b->vertices_c); ++i)
    {
        f32 s = math::dot(b->normals[i], center - (b->positions[i]));
        if (s > (a->radius)) return false;
        if (s > separation)
        {
            separation = s;
            face_normal = i;
        }
    }
    vec2 v1 = (b->positions[face_normal]);
    vec2 v2 = (b->positions[(face_normal + 1) % (b->vertices_c)]);
    // Center inside polygon check
    if (separation < math::epsilon())
    {
        m.contacts_c = 1;
        m.normal = -(b->normals[face_normal]).rotate(b->transform.orientation);
        m.contacts[0] = m.normal * (a->radius) + (a->transform.position);
        m.penetration = (a->radius);
        return true;
    }
    vec2 v1c = center - v1;
    vec2 v2c = center - v2;
    f32 dot1 = math::dot(v1c, v2 - v1);
    f32 dot2 = math::dot(v2c, v1 - v2);
    m.penetration = (a->radius) - separation;
    // Closest to v1
    if (dot1 < 0.0F)
    {
        if (v1c.lengthSq() > math::sq(a->radius)) return false;
        m.contacts_c = 1;
        m.normal = (v1 - center).rotate(b->transform.orientation).normalize();
        m.contacts[0] = v1.rotate(b->transform.orientation) + (b->transform.position);        
    }
    // Closest to v2
    else if (dot2 < 0.0F)
    {
        if (v2c.lengthSq() > math::sq(a->radius)) return false;
        m.contacts_c = 1;
        m.normal = (v2 - center).rotate(b->transform.orientation).normalize();
        m.contacts[0] = v2.rotate(b->transform.orientation) + (b->transform.position);
    }
    // Closest to face
    else
    {
        vec2 n = b->normals[face_normal];
        if (math::dot(center - v1, n) > (a->radius)) return false;
        m.contacts_c = 1;
        m.normal = -n.rotate(b->transform.orientation);
        m.contacts[0] = m.normal * (a->radius) + (a->transform.position);
    }
    return true;
}

static bool collides_polygon_circle(manifold_t& m)
{
    m.normal *= -1.0F;
    std::swap(m.a, m.b);
    return collides_circle_polygon(m);
}

static bool collides_polygon_polygon(manifold_t& m)
{
    auto support_point_fn = [](vec2* positions, u32 count, vec2 normal)
    {
        vec2 sp;
        f32 max_distance = -math::infinity();
        for (u32 i = 0; i < count; i++)
        {
            vec2 vertex = positions[i];
            f32 distance = math::dot(vertex, normal);
            if (distance > max_distance)
            {
                max_distance = distance;
                sp = vertex;
            }
        }
        return sp;
    };
    auto max_penetration_face_fn = [support_point_fn](u32& index, polygon_t* a, polygon_t* b)
    {
        f32 max_penetration = -math::infinity();
        u32 max_index;
        for (u32 i = 0; i < (a->vertices_c); ++i)
        {
            // Orient face normal with A and B modelspaces
            vec2 b_normal = a->normals[i];
            vec2 a_normal = b_normal.rotate(a->transform.orientation);
            b_normal = a_normal.rotate(-(b->transform.orientation));
            // Support point (farthest point from the normal direction)
            vec2 support = support_point_fn(b->positions, b->vertices_c, -b_normal);
            vec2 vertex = a->positions[i].rotate(a->transform.orientation) + a->transform.position;
            vertex = (vertex - (b->transform.position)).rotate(-(b->transform.orientation));
            // Compute penetration in B model space
            f32 penetration = math::dot(b_normal, support - vertex);
            // Save the deeper penetration
            if (penetration > max_penetration)
            {
                max_penetration = penetration;
                max_index = i;
            }
        }
        index = max_index;
        return max_penetration;
    };
    // Note that the vector v is assumed to be a vec2 array of length 2
    auto incident_face_fn = [](vec2* v, polygon_t* ref_polygon, polygon_t* inc_polygon, u32 ref_index)
    {
        // Transform ref_polygon normal to inc_polygon model space
        vec2 ref_normal = ref_polygon->normals[ref_index].rotate(ref_polygon->transform.orientation).rotate(-(inc_polygon->transform.orientation));
        u32 incident_face;
        // It's actually the cosine of the theta : u.v = cos(t)*|u|*|v|
        f32 min_theta = math::infinity();
        for (u32 i = 0; i < (inc_polygon->vertices_c); ++i)
        {
            f32 theta = math::dot(ref_normal, inc_polygon->normals[i]);
            if (theta < min_theta)
            {
                min_theta = theta;
                incident_face = i;
            }
        }
        // Assign the vertices of the face (transform to world space first)
        v[0] = inc_polygon->positions[incident_face++].rotate(inc_polygon->transform.orientation) + inc_polygon->transform.position;
        v[1] = inc_polygon->positions[incident_face % (inc_polygon->vertices_c)].rotate(inc_polygon->transform.orientation) + (inc_polygon->transform.position);
    };
    auto clip_fn = [](vec2 normal, f32 distance, vec2* face)
    {
        u32 clipped = 0;
        vec2 out[2] = {face[0], face[1]};
        f32 d1 = math::dot(normal, face[0]) - distance;
        f32 d2 = math::dot(normal, face[1]) - distance;
        if (d1 <= 0.0F) out[clipped++] = face[0];
        if (d2 <= 0.0F) out[clipped++] = face[1];
        if (d1 * d2 < 0.0F)
        {
            f32 alpha = d1 / (d1 - d2);
            out[clipped++] = face[0] + alpha * (face[1] - face[0]);
        }
        face[0] = out[0];
        face[1] = out[1];
        return clipped;
    };
    auto bias_greater_than = [](f32 a, f32 b)
    {
        constexpr f32 kr = 0.95f;
        constexpr f32 ka = 0.01f;
        return a >= (b * kr + a * ka);
    };

    assert((m.a->type) == polygon);
    assert((m.b->type) == polygon);
    auto a = static_cast<polygon_t*>(m.a);
    auto b = static_cast<polygon_t*>(m.b);

    m.contacts_c = 0;

    u32 face_a;
    f32 penetration_a = max_penetration_face_fn(face_a, a, b);
    if (penetration_a > 0.0F) return false;

    u32 face_b;
    f32 penetration_b = max_penetration_face_fn(face_b, b, a);
    if (penetration_b > 0.0F) return false;
    
    u32 reference_index;
    bool flip; // Always point from a to b

    polygon_t* ref; // Reference
    polygon_t* inc; // Incident

    // Determine which shape contains reference face
    if bias_greater_than(penetration_a, penetration_b))
    {
        ref = a;
        inc = b;
        reference_index = face_a;
        flip = false;
    }
    else
    {
        ref = b;
        inc = a;
        reference_index = face_b;
        flip = true;
    }

    vec2 incident_face[2];
    incident_face_fn(incident_face, ref, inc, reference_index);

    vec2 v1 = ref->positions[reference_index];
    vec2 v2 = ref->positions[(reference_index + 1) % ref->vertices_c];

    // Transform to world space
    v1 = v1.rotate(ref->transform.orientation) + (ref->transform.position);
    v2 = v2.rotate(ref->transform.orientation) + (ref->transform.position);

    vec2 side_plane_normal = (v2 - v1).normalize();
    vec2 ref_face_normal = side_plane_normal.rotateCW90();

    f32 ref_c = math::dot(ref_face_normal, v1);
    f32 neg_side = -math::dot(side_plane_normal, v1);
    f32 pos_side = math::dot(side_plane_normal, v2);

    if (clip_fn(-side_plane_normal, neg_side, incident_face) < 2) return false;
    if (clip_fn(side_plane_normal, pos_side, incident_face) < 2) return false;

    m.normal = flip ? -ref_face_normal : ref_face_normal;

    u32 cp = 0;
    f32 separation = math::dot(ref_face_normal, incident_face[0]) - ref_c;
    if (separation <= 0.0F)
    {
        m.contacts[cp++] = incident_face[0];
        m.penetration = -separation;
    }
    else m.penetration = 0.0F;

    separation = math::dot(ref_face_normal, incident_face[1]) - ref_c;
    if (separation <= 0.0F)
    {
        m.contacts[cp++] = incident_face[1];
        m.penetration -= separation;
    }
    m.contacts_c = cp;
    return true;
}

/// Polygon utility functions

static body_t compute_polygon_mass(vec2* positions, u32 count, f32 density)
{
    constexpr f32 k = (1.0F / 12.0F);
    body_t body = {};

    for (std::size_t i = 0, j = count - 1; i < count; j = i++)
    {
        vec2 a = positions[j];
        vec2 b = positions[i];
        vec2 integral = (a * a) + (a * b) + (b * b);
        f32 length = mat2 {positions[j], positions[i]}.determinant();
        body.mass += (b.x - a.x) * (b.y + a.y);
        body.moment_inertia += k * length * (integral.x + integral.y);
    }

    body.mass *= 0.5F * density * math::sign(body.mass);
    body.i_mass = 1.0F / body.mass;
    body.moment_inertia *= density * math::sign(body.moment_inertia);
    body.i_moment_inertia = 1.0F / body.moment_inertia;
    return body;
}

using collider_f = bool(*)(manifold_t&);
constexpr static collider_f collision_vtable[object_type_count][object_type_count] =
{
    {collides_circle_circle,  collides_circle_polygon },
    {collides_polygon_circle, collides_polygon_polygon},
};

static void impulse_resolution(manifold_t& m)
{
    auto a = m.a;
    auto b = m.b;

    // Function used to estimate the friction between two bodies
    auto friction_fn = [](f32 f1, f32 f2)
    {
        constexpr f32 i_sqrt2 = 0.70710678118F;
        return (f1 + f2) * i_sqrt2;
    };

    auto impulse_fn = [](object_t* obj, vec2 impulse, vec2 point)
    {
        obj->motion.velocity += (obj->body.i_mass) * impulse;
        obj->motion.omega += (obj->body.i_moment_inertia) * mat2 {point, impulse}.determinant();
    };
   
    for (u32 i = 0; i < m.contacts_c; ++i)
    {

        vec2 ra = m.contacts[i] - (a->transform.position);
        vec2 rb = m.contacts[i] - (b->transform.position);

        vec2 rv = (b->motion.velocity) + ((b->motion.omega) * rb).rotateCCW90()
                - (a->motion.velocity) - ((a->motion.omega) * ra).rotateCCW90();

        f32 speed = math::dot(rv, m.normal);

        // Objects are moving away from eachother
        if (speed > 0.0F) return;

        f32 racn = mat2 {ra, m.normal}.determinant();
        f32 rbcn = mat2 {rb, m.normal}.determinant();

        f32 i_mass_sum = (a->body.i_mass) + math::sq(racn) * (a->body.i_moment_inertia)
                       + (b->body.i_mass) + math::sq(rbcn) * (b->body.i_moment_inertia);

        // TODO: when only gravity is acting on the two bodies, make restituion zero
        f32 e = math::min(a->material.restitution, b->material.restitution);
        f32 j = -(1.0F + e) * speed / i_mass_sum; j /= f32(m.contacts_c);

        vec2 impulse = j * m.normal;

        impulse_fn(a, -impulse, ra);
        impulse_fn(b, impulse, rb);

        // Friction

        // Corner case: head-on collision produces no friction
        if (rv.lengthSq() - math::sq(speed) < math::epsilon()) continue;

        // Refresh the relative velocity before friction handling
        rv = (b->motion.velocity) + ((b->motion.omega) * rb).rotateCCW90()
           - (a->motion.velocity) - ((a->motion.omega) * ra).rotateCCW90();

        vec2 tangent = (rv - m.normal * math::dot(rv, m.normal)).normalize();
        f32 jt = -math::dot(rv, tangent) / i_mass_sum; jt /= f32(m.contacts_c);

        f32 sf = friction_fn(a->material.static_coef, b->material.static_coef);
        if (math::abs(jt) < j * sf)
            impulse = jt * tangent;
        else
        {
            f32 df = friction_fn(a->material.dynamic_coef, b->material.dynamic_coef);
            impulse = -j * tangent * df;
        }

        impulse_fn(a, -impulse, ra);
        impulse_fn(b, impulse, rb);

    }
}

static void positional_correction(manifold_t& m)
{
    constexpr f32 percent = 1.0F;
    constexpr f32 slop = 0.25F;
    vec2 correction = percent * m.normal * math::max(m.penetration - slop, 0.0F);
    f32 t = m.a->body.i_mass / (m.a->body.i_mass + m.b->body.i_mass);
    m.a->transform.position -= t * correction;
    m.b->transform.position += (1.0F - t) * correction;
}
static void positional_correction_s(manifold_t& m)
{
    constexpr f32 percent = 1.0F;
    constexpr f32 slop = 0.25F;
    vec2 correction = percent * m.normal * math::max(m.penetration - slop, 0.0F);
    m.a->transform.position -= correction;
}

/// Engine class implementation

Physics2D::Physics2D(std::size_t max_objects, f32 timestep)
    : m_max_objects {max_objects}, m_timestep {timestep}, m_half_timestep {timestep * 0.5F}, m_gravity {0.0F, 0.0F}
{
    m_dynamic.reserve(max_objects);
    m_static.reserve(max_objects);
}

circle_t* Physics2D::add(const transform_t& transform, const material_t& material, const motion_t& motion, f32 density, f32 radius)
{
    assert(m_dynamic.size() < m_max_objects);
    shape_t shape;
    shape.c.type = object_type_t::circle;
    shape.c.transform = transform;
    shape.c.material = material;
    shape.c.motion = motion;
    shape.c.radius = radius;
    shape.c.body.mass = math::pi() * math::sq(radius) * density;
    shape.c.body.i_mass = 1.0F / shape.c.body.mass;
    shape.c.body.moment_inertia = 0.5F * math::pi() * math::pow(radius, 4.0F) * density;
    shape.c.body.i_moment_inertia = 1.0F / shape.c.body.moment_inertia;
    if (shape.c.body.i_mass != 0.0F)
    {
        m_dynamic.emplace_back(shape);
        return static_cast<circle_t*>(&m_dynamic.back().c);
    }
    else
    {
        m_static.emplace_back(shape);
        return static_cast<circle_t*>(&m_static.back().c);
    }
}

polygon_t* Physics2D::add(const transform_t& transform, const material_t& material, const motion_t& motion, f32 density, vec2* positions, vec2* normals, u32 vertices_c)
{
    assert(m_dynamic.size() < m_max_objects);
    shape_t shape;
    shape.p.type = object_type_t::polygon;
    shape.p.transform = transform;
    shape.p.material = material;
    shape.p.motion = motion;
    shape.p.body = compute_polygon_mass(positions, vertices_c, density);
    shape.p.vertices_c = vertices_c;
    shape.p.positions = positions;
    shape.p.normals = normals;
    if (shape.p.body.i_mass != 0.0F)
    {
        m_dynamic.emplace_back(shape);
        return static_cast<polygon_t*>(&m_dynamic.back().p);
    }
    else
    {
        m_static.emplace_back(shape);
        return static_cast<polygon_t*>(&m_static.back().p);
    }
}

void Physics2D::simulate()
{
    manifold_t manifold;

    for (auto i = m_dynamic.begin(); i != m_dynamic.end(); ++i)
    {
        for (auto j = (i + 1); j != m_dynamic.end(); ++j)
        {
            manifold.a = &i->o;
            manifold.b = &j->o;

            // TODO: broadphasing
            // TODO: layering

            if (collision_vtable[manifold.a->type][manifold.b->type](manifold))
            {
                impulse_resolution(manifold);
                positional_correction(manifold);
            }
        }
        for (auto j = m_static.begin(); j != m_static.end(); ++j)
        {
            manifold.a = &i->o;
            manifold.b = &j->o;

            if (collision_vtable[manifold.a->type][manifold.b->type](manifold))
            {
                impulse_resolution(manifold);
                positional_correction_s(manifold);
            }
        }
        // Linear motion integration
        i->o.motion.velocity += ((i->o.motion.force) * (i->o.body.i_mass) + m_gravity) * m_half_timestep;
        i->o.transform.position += (i->o.motion.velocity) * m_timestep;
        i->o.motion.force = {};
        // Angular motion integration
        i->o.motion.omega += (i->o.motion.torque) * (i->o.body.i_moment_inertia) * m_half_timestep;
        i->o.transform.orientation += (i->o.motion.omega) * m_timestep;
        i->o.motion.torque = {};
    }
}

f32 Physics2D::interval() const
{
    return m_timestep;
}

u32 Physics2D::entities() const
{
    return m_dynamic.size();
}

u32 Physics2D::capacity() const
{
    return m_max_objects;
}

vec2& Physics2D::gravity()
{
    return m_gravity;
}

void Physics2D::for_each_object(object_callback_t callback)
{
    for (auto& blob : m_dynamic) callback(blob.o);
    for (auto& blob : m_static)  callback(blob.o);
}

void Physics2D::for_each_object(const_object_callback_t callback) const
{
    for (auto& blob : m_dynamic) callback(blob.o);
    for (auto& blob : m_static)  callback(blob.o);
}

}
