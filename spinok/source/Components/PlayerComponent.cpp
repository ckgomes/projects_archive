#include "PlayerComponent.hpp"
#include "PhysicsComponent.hpp"
#include "Transform2DComponent.hpp"

static constexpr s32 jumpSteps = 6;
static constexpr vec2 jumpForce = {0.0F, 55.0F};
static constexpr f64 jumpInterval = 0.75;

static constexpr f32 walkVelocity = 5.5F;
static constexpr f32 horiziontalAcceleration = 9.25F;

PlayerComponent::PlayerComponent(Entity* parent, ImmediateRenderer2D* renderer, Keyboard* keyboard)
    : Component(parent), m_renderer(*renderer), m_keyboard(*keyboard), steps(0), m_jumped(0.0)
{
    assert(p->hasComponent<PhysicsComponent>());
    assert(p->hasComponent<Transform2DComponent>());

    auto transform = p->getComponent<Transform2DComponent>();
    auto physics = p->getComponent<PhysicsComponent>();

    b2Body* body = physics->getBody();
    b2Fixture* fixture = body->GetFixtureList();

    b2Vec2 sensorDim = body->GetFixtureList()->GetAABB(0).GetExtents();
    b2PolygonShape poly;
    b2FixtureDef fixDef;

    poly.SetAsBox(sensorDim.x * 0.5F, sensorDim.y * 0.3F, b2Vec2 {0.0F, -sensorDim.y * 1.15F}, 0.0F);
    fixDef.isSensor = true;
    fixDef.shape = &poly;
    fixDef.userData = &m_contacts;
    fixDef.filter = fixture->GetFilterData();
    body->CreateFixture(&fixDef);

    m_renderer.assignCamera(&transform->position);

}

PlayerComponent::~PlayerComponent()
{
    m_renderer.assignCamera(nullptr);
}

void PlayerComponent::update()
{

    auto transform = p->getComponent<Transform2DComponent>();
    auto physics = p->getComponent<PhysicsComponent>();

    f64 now = timer.elapsed();
    vec2 velocity = physics->getVelocity();

    b2Body* body = physics->getBody();
    b2Fixture* sensor = body->GetFixtureList();
    b2Fixture* player = sensor->GetNext();

    // When the key 's' is pressed, the player no longer collides with green tiles, making them passable    
    b2Filter filter = player->GetFilterData();
    if (m_keyboard['s']) filter.maskBits &= ~GameEntities::LEDGE_BLOCK;
    else filter.maskBits |= GameEntities::LEDGE_BLOCK;
    player->SetFilterData(filter);

    if (m_keyboard.isDown('w') && (m_contacts > 0) && (now - m_jumped > jumpInterval))
    {
        m_jumped = now;
        steps = jumpSteps;
    }

    if (m_keyboard['d'])
    {
        if (velocity.x < walkVelocity)
            body->ApplyForceToCenter({horiziontalAcceleration, 0.0F}, true);
    }

    if (m_keyboard['a'])
    {
        if (velocity.x > -walkVelocity)
            body->ApplyForceToCenter({-horiziontalAcceleration, 0.0F}, true);
    }

    if (steps > 0)
    {
        physics->applyForce(jumpForce);
        steps--;
    }
}
