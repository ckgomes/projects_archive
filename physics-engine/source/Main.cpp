#include <SDL2/SDL.h>

#ifdef _WIN32
#include <Windows.h>
#include <gl/GL.h>
#else
#error Unsupported platform
#endif

#include "Math.hpp"
#include "Timer.hpp"
#include "Physics.hpp"

#include <vector>

using namespace PHYSICS_NAMESPACE;

struct Mouse
{
    enum { LEFT = 1, MIDDLE, RIGHT };
    bool buttons[4];
    f32 dx, dy;
    f32 x, y;
};

void draw_vector(vec2 a, vec2 b)
{
    vec2 v = b - a;
    vec2 cw45 = v.rotate(0.25F * math::pi()).normalize() * v.length() * 0.3F;
    vec2 ccw45 = v.rotate(-0.25F * math::pi()).normalize() * v.length() * 0.3F;
    // White
    glColor3f(1.0F, 1.0F, 1.0F);
    glBegin(GL_LINES);
    glVertex2f(a.x, a.y);
    glVertex2f(a.x + v.x, a.y + v.y);
    glVertex2f(a.x + v.x, a.y + v.y);
    glVertex2f(a.x + v.x - cw45.x, a.y + v.y - cw45.y);
    glVertex2f(a.x + v.x, a.y + v.y);
    glVertex2f(a.x + v.x - ccw45.x, a.y + v.y - ccw45.y);
    glEnd();
}

void draw_circle(const object_t& o)
{
    constexpr int n = 25;
    constexpr f32 pi2 = math::pi() * 2.0F;
    constexpr f32 step = pi2 / f32(n);

    auto circle = static_cast<const circle_t&>(o);
    // Light blue
    glColor3f(0.5F, 0.5F, 1.0F);
    glLineWidth(3.0F);
    glBegin(GL_LINE_LOOP);
    for (f32 theta = 0.0F; theta < pi2; theta += step)
    {
        f32 sin = math::sin(theta);
        f32 cos = math::cos(theta);
        vec2 vertex = circle.transform.position + vec2 {cos, sin} * circle.radius;
        glVertex2f(vertex.x, vertex.y);
    }
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(o.transform.position.x, o.transform.position.y);
    vec2 r = o.transform.position + vec2 {circle.radius, 0.0F}.rotate(o.transform.orientation);
    glVertex2f(r.x, r.y);
    glEnd();
    glPointSize(5.0F);
    glBegin(GL_POINTS);
    glVertex2f(o.transform.position.x, o.transform.position.y);
    glEnd();
}

void draw_polygon(const object_t& o)
{
    auto polygon = static_cast<const polygon_t&>(o);
    glLineWidth(2.0F);
    glColor3f(1.0F, 1.0F, 1.0F);
    glPushMatrix();
    glTranslatef(polygon.transform.position.x, polygon.transform.position.y, 0.0F);
    glRotatef(polygon.transform.orientation * 180.0F / math::pi(), 0.0F, 0.0F, 1.0F);
    for (std::size_t i = 0, j = polygon.vertices_c - 1; i < polygon.vertices_c; j = i++)
    {
        vec2 a = polygon.positions[j];
        vec2 b = polygon.positions[i];
        vec2 n = polygon.normals[j];
        vec2 m = (a + b) * 0.5F;
        //draw_vector(m, m + n * 17.5F);
    }
    glBegin(GL_LINE_LOOP);
    glColor3f(0.0F, 1.0F, 0.0F);
    for (std::size_t i = 0; i < polygon.vertices_c; i++)
    {
        vec2 p = polygon.positions[i];
        glVertex2f(p.x, p.y);
    }
    glEnd();
    glColor3f(1.0F, 0.0F, 0.0F);
    glPointSize(5.0F);
    glBegin(GL_POINTS);
    glVertex2f(0.0F, 0.0F);
    glEnd();
    glPopMatrix();
}

void draw_object(const object_t& o)
{
    // This is what a vtable looks like
    static void(*jt[])(const object_t&) = {draw_circle, draw_polygon};
    jt[u32(o.type)](o);
}

constexpr f32 width = 800.0F;
constexpr f32 height = 600.0F;

Physics2D p(1000, 0.01F);

polygon_t* playa = nullptr;
polygon_t* enemy = nullptr;

gfx::Mesh ast({vec2{1.0F, 2.0F}});

void render(f64 dt, bool* kb, const Mouse& m)
{
    constexpr f32 speed = 300.0F;
    constexpr f32 max_speed = math::sq(500.0F);

    static f64 accumulator = 0.0;
    accumulator += dt;

    static bool allow = true;
    if (m.buttons [Mouse::LEFT] && allow)
    {   
        if (math::random(0, 1))
        {
            p.add(transform_t {vec2 {m.x, height - m.y}, 0.0F, 1.0F},
                  material_t {1.0F, 0.5F, 0.3F},
                  motion_t {},
                  1.0F, 25.0F);
        }
        else
        {
            p.add(transform_t {vec2 {m.x, height - m.y}},
                  material_t {0.1F, 0.5F, 0.3F},
                  motion_t {},
                  5.0F,
                  &ast.positions().front(),
                  &ast.normals().front(),
                   ast.vertices());
        }
    }
    allow = !m.buttons [Mouse::LEFT];

    if (kb ['w']) playa->motion.velocity.y += speed * f32(dt);
    if (kb ['s']) playa->motion.velocity.y -= speed * f32(dt);
    if (kb ['d']) playa->motion.velocity.x += speed * f32(dt);
    if (kb ['a']) playa->motion.velocity.x -= speed * f32(dt);

    while (accumulator > 0.0)
    {
        p.simulate();       
        accumulator -= p.interval();
    }

    p.for_each_object(draw_object);
}

int main(int argc, char** argv)
{

    bool quit = false;
    bool keyboard[255] = {};
    Mouse mouse = {};
    Timer timer;

    auto window = SDL_CreateWindow("Physics 2D engine",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   (int) width,
                                   (int) height,
                                   SDL_WINDOW_OPENGL);

    auto context = SDL_GL_CreateContext(window);

    glViewport(0, 0, (i32) width, (i32) height);
    glMatrixMode(GL_PROJECTION);
    glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    std::vector<vec2> pppoe =
    {
        vec2 { 100.0F,  100.0F} * 0.5F,
        vec2 { 100.0F, -100.0F} * 0.5F,
        vec2 {-100.0F, -100.0F} * 0.5F,
        vec2 {-100.0F,  100.0F} * 0.5F
    };

    std::vector<vec2> box =
    {
        vec2 { 400.0F,  1.0F},
        vec2 { 400.0F, -1.0F},
        vec2 {-400.0F, -1.0F},
        vec2 {-400.0F,  1.0F}
    };

    gfx::Mesh poly(std::move(pppoe));
    gfx::Mesh boxp(std::move(box));

    std::vector<vec2> asteroid_like;
    f32 bias = 75.0F;
    for (f32 i = 0.0F; i < math::pi() * 2.0F; i += (math::pi() * 2.0F) / 20.0F)
    {
        bias += math::random(-6.0F, 6.0F);
        f32 cos = math::cos(i);
        f32 sin = math::sin(i);
        asteroid_like.emplace_back(vec2 {cos, sin} * bias);
    }
    ast = gfx::Mesh(std::move(asteroid_like));

    playa = p.add(transform_t {vec2{100.0F, 100.0F}, 0.0F, 1.0F},
                  material_t {1.0F, 0.5F, 0.3F},
                  motion_t {},
                  1.0F,
                  &poly.positions().front(),
                  &poly.normals().front(),
                  poly.vertices());

    p.add(transform_t{vec2 {width * 0.5F, height}, 0.0F * math::pi(), 1.0F},
                  material_t{0.1F, 0.5F, 0.3F},
                  motion_t{},
                  math::infinity(),
                  &boxp.positions().front(),
                  &boxp.normals().front(),
                  boxp.vertices());

    p.add(transform_t {vec2 {width * 0.5F, 0.0F}, 0.0F * math::pi(), 1.0F},
          material_t {0.1F, 0.5F, 0.3F},
          motion_t {},
          math::infinity(),
          &boxp.positions().front(),
          &boxp.normals().front(),
          boxp.vertices());

    p.add(transform_t {vec2 {0.0F, height * 0.5F}, 0.5F * math::pi(), 1.0F},
          material_t {0.1F, 0.5F, 0.3F},
          motion_t {},
          math::infinity(),
          &boxp.positions().front(),
          &boxp.normals().front(),
          boxp.vertices());

    p.add(transform_t {vec2 {width, height * 0.5F}, 0.5F * math::pi(), 1.0F},
          material_t {0.1F, 0.5F, 0.3F},
          motion_t {},
          math::infinity(),
          &boxp.positions().front(),
          &boxp.normals().front(),
          boxp.vertices());

    p.add(transform_t {vec2 {0.0F, height * 0.5F}, 0.25F * math::pi(), 1.0F},
          material_t {0.1F, 0.5F, 0.3F},
          motion_t {},
          math::infinity(),
          &boxp.positions().front(),
          &boxp.normals().front(),
          boxp.vertices());

    p.add(transform_t {vec2 {width, height * 0.5F}, 0.25F * math::pi(), 1.0F},
          material_t {0.1F, 0.5F, 0.3F},
          motion_t {},
          math::infinity(),
          &boxp.positions().front(),
          &boxp.normals().front(),
          boxp.vertices());
    
    p.gravity() = {0.0F, -100.0F};
    timer.reset();

    SDL_Event e;
    while (!quit && !keyboard [27])
    {
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {

            case SDL_QUIT:
                quit = true;
                break;

            case SDL_KEYDOWN:
                if (e.key.keysym.sym < 255) keyboard [e.key.keysym.sym] = true;
                break;

            case SDL_KEYUP:
                if (e.key.keysym.sym < 255) keyboard[e.key.keysym.sym] = false;
                break;

            case SDL_MOUSEMOTION:
                mouse.x = f32(e.motion.x);
                mouse.y = f32(e.motion.y);
                mouse.dx = f32(e.motion.xrel);
                mouse.dy = f32(e.motion.yrel);
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button < 4) mouse.buttons[e.button.button] = true;
                break;

            case SDL_MOUSEBUTTONUP:
                if (e.button.button < 4) mouse.buttons [e.button.button] = false;
                break;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);
        render(timer.interval(), keyboard, mouse);
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    return 0;
}