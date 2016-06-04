#ifndef MESH_COMPONENT_HPP
#define MESH_COMPONENT_HPP

#include "../ECS/Component.hpp"

class HullComponent final : public Component
{

public:

    HullComponent(Entity* parent, const MeshManager* manager, u32 meshid);

    void update() override;

    u32 getMeshID() const;

private:

    const MeshManager& m_manager;
    u32 m_meshid;

};

#endif // MESH_COMPONENT_HPP
