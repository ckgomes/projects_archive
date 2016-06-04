#include "../Common/Configuration.hpp"

#include <Box2D/Box2D.h>

#include "../Game/GameTypes.hpp"
#include "../Game/Map.hpp"

#include <iostream>

class ContactListener : public b2ContactListener
{

    void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override
    {
        if (!contact->GetFixtureA()->IsSensor())
        {
            u16* type = reinterpret_cast<u16*>(contact->GetFixtureA()->GetUserData());
            if (*type == GameEntities::LEDGE_BLOCK)
            {
                b2WorldManifold worldspace;
                contact->GetWorldManifold(&worldspace);
                contact->SetEnabled(worldspace.normal.y > mm::epsilon());
            }
        }
    }

    void BeginContact(b2Contact* contact) override
    {
        if (contact->GetFixtureB()->IsSensor())
        {
            size_t* contacts = static_cast<size_t*>(contact->GetFixtureB()->GetUserData());
            ++(*contacts);
        }
    }

    void EndContact(b2Contact* contact) override
    {
        if (contact->GetFixtureB()->IsSensor())
        {
            size_t* contacts = static_cast<size_t*>(contact->GetFixtureB()->GetUserData());
            --(*contacts);
        }
    }

};