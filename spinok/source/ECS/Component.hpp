#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "../Common/Configuration.hpp"

class Entity;

class Component
{

public:

    virtual void update() = 0;

    Component(Entity* parent);

    bool isActive() const;
    void setActive(bool active);

    friend class Entity;

protected:

    Entity* p;
    bool active;

};

#endif // COMPONENT_HPP
