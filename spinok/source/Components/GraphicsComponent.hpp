#ifndef  GRAPHICS_COMPONENT_HPP
#define  GRAPHICS_COMPONENT_HPP

#include "../ECS/Component.hpp"
#include "../Services/ImmediateRenderer2D.hpp"
#include "../Services/DataManager.hpp"

class GraphicsComponent final : public Component
{

public:

    GraphicsComponent(Entity* parent, ImmediateRenderer2D* renderer, const MeshManager* manager);

    void update() override;

private:

    ImmediateRenderer2D& m_renderer;
    const MeshManager& m_manager;

};

#endif // GRAPHICS_COMPONENT_HPP
