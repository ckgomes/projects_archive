#ifndef IMMEDIATE_RENDERER2D_HPP
#define IMMEDIATE_RENDERER2D_HPP

#include <vector>
#include <string>

#include "Window.hpp"
#include "Timer.hpp"
#include "Font.hpp"

#include "../Common/Vector2.hpp"

// Uses an immediate OpenGL context to render
// The main purpose of this renderer is debugging and fast prototyping
// However it can still be used for simple applications that don't require scalability
class ImmediateRenderer2D final
{

public:

    // Creates a rendered that uses immediate mode in OpenGL
    // The window is expected to have valid OpenGL context
    // If buildOrthographicMatrix is true an orthographic projection matrix will be created that will match the dimensions of the window
    ImmediateRenderer2D(const Window& window, bool buildOrthographicMatrix = true);
    ~ImmediateRenderer2D();

    // Replaces the current projection matrix by an orthographic matrix
    void orthographic(f64 left, f64 right, f64 bottom, f64 top);

    void setViewport(s32 width, s32 height);

    // Changes the color used by the rasterizer
    void setColor(f32 red, f32 green, f32 blue);

    void assignCamera(const vec2* camera);

    s32 getFrameRate() const;

    // The dimensions of the viewport are not to be confused with the dimensions of the window
    // These are the dimensions established by the orthographic projection matrix
    vec2 viewportDimensions() const;

    // Pushes the transformation into the matrix stack
    void pushTransform(vec2 position, f32 orientation, f32 scale);
    void pushTransform(vec2 position, f32 orientation, vec2 scale);
    // Removes the top transformation from the matrix stack
    // If there are no transformations on the matrix stack this method will do nothing
    void popTransform();

    // Loads a texture from file and returns it's ID
    u32 loadTexture(const char* path) const;

    // Rasterizes a point
    void drawPoint(vec2 v);
    // Rasterizes a line between the points
    void drawLine(vec2 v1, vec2 v2);
    // Fills a triangle between the points
    void drawTriangle(vec2 v1, vec2 v2, vec2 v3);
    // Rasterizes a hull described by the position of the vertices
    // A polygon is a filled shape, while a hull is the contour, which looks similar to wireframe renderings
    void drawHull(const std::vector<vec2>& positions);
    // Rasterizes a polygon described by the position of the vertices
    // A polygon is a filled shape, while a hull is the contour, which looks similar to wireframe renderings
    void drawPolygon(const std::vector<vec2>& positions);
    // Rasterizes a polygon with positions described by the indices
    // A polygon is a filled shape, while a hull is the contour, which looks similar to wireframe renderings
    void drawPolygon(const std::vector<vec2>& positions, const std::vector<u16>& indices);

    // Rasterizes a textured quad at the specified coordinates
    void drawTexture(u32 texture, vec2 min, vec2 max);

    void drawText(const Font& font, const std::string& text, vec2 position, f32 px = 16.0F);

    // Rasterizes a quad at the specified coordinates
    void drawBox(vec2 min, vec2 max);
    // Rasterizes a circle at the specified coordinates
    void drawCircle();

    // Clears the screen and the modelview matrix stack
    // Centers the camera in the specified location
    // Note that this will not clear the projection matrix
    void prepare();
    // Presents the current framebuffer to the screen
    void present();

private:

    Timer m_timer;
    u64 m_frame;
    s32 m_fps;

    const vec2* m_camera;

    const Window& m_window;
    u32 m_stack_depth;

    vec2 m_viewport_center;

};

#endif//IMMEDIATE_RENDERER2D_HPP
