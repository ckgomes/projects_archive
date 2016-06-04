#ifndef PHYSICS_COMPONENT_HPP
#define PHYSICS_COMPONENT_HPP

#include "../ECS/Component.hpp"

#include "../Common/Vector2.hpp"

#include "../Game/GameTypes.hpp"

#include <Box2D/Box2D.h>

class PhysicsComponent final : public Component
{

public:

    enum Type
    {
        Static,
        Dynamic
    };

    PhysicsComponent(Entity* parent, b2World* world, const MeshManager* manager, GameEntities::Type category, u16 collidesWith, Type type = Dynamic);
    ~PhysicsComponent();

    vec2 getVelocity() const;
    void setVelocity(vec2 velocity);
    void addVelocity(vec2 velocity);

    void applyForce(vec2 force);

    void update() override;

    b2Body* getBody();

private:

    b2Body* m_body;

};

#endif // PHYSICS_COMPONENT_HPP
