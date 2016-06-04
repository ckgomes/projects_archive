#include "PhysicsComponent.hpp"
#include "Transform2DComponent.hpp"
#include "HullComponent.hpp"

#include "../ECS/Entity.hpp"

#include "../Services/DataManager.hpp"

PhysicsComponent::PhysicsComponent(Entity* parent, b2World* world, const MeshManager* manager, GameEntities::Type category, u16 collidesWith, Type type)
    : Component (parent)
{
    assert(p->hasComponent<HullComponent>());
    assert(p->hasComponent<Transform2DComponent>());

    auto hull = p->getComponent<HullComponent>();
    auto transform = p->getComponent<Transform2DComponent>();

    std::vector<vec2> mesh = manager->at(hull->getMeshID());
    for (vec2& vertex : mesh) vertex *= transform->scale;
    b2Vec2* vertices = reinterpret_cast<b2Vec2*>(&mesh.front());

    b2BodyDef body;
    body.position = {transform->position.x, transform->position.y};
    body.angle = transform->orientation;
    body.type = (type == Dynamic) ? b2_dynamicBody : b2_staticBody;

    b2PolygonShape poly;
    poly.Set(vertices, mesh.size());

    b2FixtureDef fix;
    fix.shape = &poly;
    fix.restitution = 0.0F;
    fix.isSensor = false;
    fix.density = 1.0F;
    fix.filter.categoryBits = category;
    fix.filter.maskBits = collidesWith;

    m_body = world->CreateBody(&body);
    m_body->CreateFixture(&fix);

}

PhysicsComponent::~PhysicsComponent()
{
    b2World* world = m_body->GetWorld();
    world->DestroyBody(m_body);
}

vec2 PhysicsComponent::getVelocity() const
{
    b2Vec2 velocity = m_body->GetLinearVelocity();
    return {velocity.x, velocity.y};
}

void PhysicsComponent::setVelocity(vec2 velocity)
{
    m_body->SetLinearVelocity({velocity.x, velocity.y});
}

void PhysicsComponent::addVelocity(vec2 velocity)
{
    setVelocity(getVelocity() + velocity);
}

void PhysicsComponent::applyForce(vec2 force)
{
    m_body->ApplyForceToCenter({force.x, force.y}, true);
}

void PhysicsComponent::update()
{
    auto transform = p->getComponent<Transform2DComponent>();
    b2Vec2 position = m_body->GetPosition();
    f32 orientation = m_body->GetAngle();
    transform->position = {position.x, position.y};
    transform->orientation = orientation;
}

b2Body* PhysicsComponent::getBody()
{
    return m_body;
}
