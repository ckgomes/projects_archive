#include "ImmediateRenderer2D.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <gl/GL.h>
#else
#error Unsupported platform
#endif

#include <SDL2/SDL_image.h>

#include <cassert>
#include <algorithm>

constexpr static f32 toDegree = 180.0F / mm::pi();

ImmediateRenderer2D::ImmediateRenderer2D(const Window& window, bool buildOrthographicMatrix)
    : m_window(window), m_stack_depth(0), m_camera (nullptr)
{
    assert (window.hasGLContext());
    
    s32 width, height;
    window.getResolution(width, height);

    if (buildOrthographicMatrix) orthographic(0.0, f64(width), 0.0, f64(height));
    else glMatrixMode(GL_MODELVIEW);

    glLineWidth(2.0F);
    glPointSize(5.0F);
    glClearColor(1.0F, 1.0F, 0.5F, 1.0F);

    glViewport(0, 0, width, height);

    glEnable(GL_TEXTURE_2D);

}

ImmediateRenderer2D::~ImmediateRenderer2D()
{
    while (m_stack_depth > 0)
    {
        glPopMatrix();
        m_stack_depth--;
    }
}

void ImmediateRenderer2D::orthographic(f64 left, f64 right, f64 bottom, f64 top)
{
    m_viewport_center = vec2 {f32(right - left), f32(top - bottom)} * 0.5F;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(left, right, bottom, top, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void ImmediateRenderer2D::setViewport(s32 width, s32 height)
{
    glViewport(0, 0, width, height);
}

void ImmediateRenderer2D::setColor(f32 red, f32 green, f32 blue)
{
    glColor3f(red, green, blue);
}

void ImmediateRenderer2D::assignCamera(const vec2* camera)
{
    m_camera = camera;
}

void ImmediateRenderer2D::pushTransform(vec2 position, f32 orientation, f32 scale)
{
    pushTransform(position, orientation, {scale, scale});
}

void ImmediateRenderer2D::pushTransform(vec2 position, f32 orientation, vec2 scale)
{
    glPushMatrix();
    glTranslatef(position.x, position.y, 0.0F);
    glScalef(scale.x, scale.y, 1.0F);
    glRotatef(orientation * toDegree, 0.0F, 0.0F, 1.0F);
    m_stack_depth++;
}

void ImmediateRenderer2D::popTransform()
{
    if (m_stack_depth > 0)
    {
        glPopMatrix();
        m_stack_depth--;
    }
}

u32 ImmediateRenderer2D::loadTexture(const char* path) const
{

    constexpr u32 textureFormats[] =
    {
        GL_INVALID_VALUE,
        GL_LUMINANCE,
        GL_INVALID_VALUE,
        GL_RGB,
        GL_RGBA
    };

    u32 textureid;
    glGenTextures(1, &textureid);

    auto image = IMG_Load(path);
    assert (image != nullptr);
    defer(SDL_FreeSurface(image));

    glBindTexture(GL_TEXTURE_2D, textureid);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, textureFormats[image->format->BytesPerPixel], GL_UNSIGNED_BYTE, image->pixels);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureid;
}

void ImmediateRenderer2D::drawPoint(vec2 v)
{
    glBegin(GL_POINTS);
    glVertex2f(v.x, v.y);
    glEnd();
}

void ImmediateRenderer2D::drawLine(vec2 v1, vec2 v2)
{
    glBegin(GL_LINES);
    glVertex2f(v1.x, v1.y);
    glVertex2f(v2.x, v2.y);
    glEnd();
}

void ImmediateRenderer2D::drawTriangle(vec2 v1, vec2 v2, vec2 v3)
{
    glBegin(GL_TRIANGLES);
    glVertex2f(v1.x, v1.y);
    glVertex2f(v2.x, v2.y);
    glVertex2f(v3.x, v3.y);
    glEnd();
}

void ImmediateRenderer2D::drawHull(const std::vector<vec2>& positions)
{
    glBegin(GL_LINE_LOOP);
    for (const auto& position : positions)
        glVertex2f(position.x, position.y);
    glEnd();
}

void ImmediateRenderer2D::drawPolygon(const std::vector<vec2>& positions)
{
    glBegin(GL_POLYGON);
    for (const auto& position : positions)
        glVertex2f(position.x, position.y);
    glEnd();
}

void ImmediateRenderer2D::drawPolygon(const std::vector<vec2>& positions, const std::vector<u16>& indices)
{
    glBegin(GL_TRIANGLES);
    for (u16 index : indices)
    {
        const vec2& vertex = positions[index];
        glVertex2f(vertex.x, vertex.y);
    }
    glEnd();
}

void ImmediateRenderer2D::drawTexture(u32 texture, vec2 min, vec2 max)
{

    f32 color[4];
    glGetFloatv(GL_CURRENT_COLOR, color);
    glColor3f(1.0F, 1.0F, 1.0F);

    glBindTexture(GL_TEXTURE_2D, texture);
    glPushMatrix();
    glLoadIdentity();
    glBegin(GL_QUADS);
    glTexCoord2f(0.0F, 0.0F); glVertex2f(min.x, max.y);
    glTexCoord2f(1.0F, 0.0F); glVertex2f(max.x, max.y);
    glTexCoord2f(1.0F, 1.0F); glVertex2f(max.x, min.y);
    glTexCoord2f(0.0F, 1.0F); glVertex2f(min.x, min.y);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);

    glColor3f(color[0], color[1], color[2]);

}

void ImmediateRenderer2D::drawText(const Font& font, const std::string& text, vec2 position, f32 px)
{
    f32 w, h;
    m_window.getResolution(w, h);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
    glBegin(GL_TRIANGLES);
    for (char ch : text)
    {
        const auto& vertices = font.positions(ch);
        for (u16 index : font.indices(ch))
        {
            vec2 vertex = (vertices[index] * px) + position;
            glVertex2f(vertex.x, vertex.y);
        }
        position.x += font.advancement(ch) * px;
    }
    glEnd();
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void ImmediateRenderer2D::drawBox(vec2 min, vec2 max)
{
    glBegin(GL_QUADS);
    glVertex2f(min.x, min.y);
    glVertex2f(min.x, max.y);
    glVertex2f(max.x, max.y);
    glVertex2f(max.x, min.y);
    glEnd();
}

void ImmediateRenderer2D::drawCircle()
{

    constexpr u32 quality = 20;
    constexpr f32 step = mm::pi(2.0F) / f32(quality);

    f32 theta = 0.0F;

    glBegin(GL_LINE_LOOP);
    for (u32 i = 0; i < quality; i++, theta += step)
    {
        f32 cos = mm::cos(theta);
        f32 sin = mm::sin(theta);
        glVertex2f(cos, sin);
    }
    glEnd();
}

void ImmediateRenderer2D::prepare()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    while (m_stack_depth > 0)
    {
        glPopMatrix();
        m_stack_depth--;
    }

    vec2 cam = {};
    if (m_camera != nullptr)
    {
        cam = m_viewport_center - (*m_camera);
    }

    f32 viewMatrix[16] =
    {
        1.0F, 0.0F, 0.0F, 0.0F,
        0.0F, 1.0F, 0.0F, 0.0F,
        0.0F, 0.0F, 1.0F, 0.0F,
        cam.x, cam.y, 0.0F, 1.0F
    };

    glLoadMatrixf(viewMatrix);
    m_frame++;
}

void ImmediateRenderer2D::present()
{
    assert(glGetError() == GL_NO_ERROR);
    m_window.present();

    if (m_frame % 60 == 0)
    {
        m_fps = s32(m_timer.elapsed() * 60.0F);
        m_timer.reset();
    }
}

s32 ImmediateRenderer2D::getFrameRate() const
{
    return m_fps;
}

vec2 ImmediateRenderer2D::viewportDimensions() const
{
    return m_viewport_center * 2.0F;
}
