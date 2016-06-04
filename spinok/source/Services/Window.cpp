#include "Window.hpp"

#include <cstdlib>
#include <cassert>
#include <algorithm>

#include <SDL2/SDL.h>

#include "../Common/Math.hpp"

static SDL_Window* cast_wnd(void* window) noexcept
{
    return static_cast<SDL_Window*>(window);
}

Window::Window(const char* title, s32 width, s32 height)
{
    SDL_Window* window;
    SDL_GLContext context;

    if (!SDL_WasInit(SDL_INIT_EVERYTHING))
    {
        SDL_Init(SDL_INIT_EVERYTHING);
        atexit(SDL_Quit);
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    assert(window != nullptr);

    context = SDL_GL_CreateContext(window);
    assert(context != nullptr);

    SDL_GL_SetSwapInterval(-1);

    m_window  = window;
    m_context = context;

    setResolution(width, height);

}

Window::~Window()
{
    SDL_Window* window = cast_wnd(m_window);
    SDL_GLContext context = static_cast<SDL_GLContext>(m_context);
    if (m_context != nullptr) SDL_GL_DeleteContext(context);
    if (m_window  != nullptr) SDL_DestroyWindow(window);
}

Window::Window(Window&& window)
{
    std::swap(m_width, window.m_width);
    std::swap(m_height, window.m_height);
    std::swap(m_window, window.m_window);
    std::swap(m_context, window.m_context);
}

const Window& Window::operator=(Window&& window)
{
    std::swap(m_width, window.m_width);
    std::swap(m_height, window.m_height);
    std::swap(m_window, window.m_window);
    std::swap(m_context, window.m_context);
    return (*this);
}

void Window::setTitle(const char* title)
{
    SDL_Window* window = cast_wnd(m_window);
    SDL_SetWindowTitle(window, title);
}

void Window::setResolution(s32 width, s32 height)
{
    SDL_Window* window = cast_wnd(m_window);
    m_width  = f32(width);
    m_height = f32(height);
    SDL_SetWindowSize(window, width, height);
}

void Window::getResolution(f32& width, f32& height) const
{
    width  = m_width;
    height = m_height;
}

void Window::getResolution(s32& width, s32& height) const
{
    width  = s32(m_width  + 0.5F);
    height = s32(m_height + 0.5F);
}

f32 Window::aspectRatio() const
{
    return mm::max(m_width / m_height, m_height / m_width);
}

bool Window::hasGLContext() const noexcept
{
    return (m_context != nullptr);
}

bool Window::update(Keyboard& kb, Mouse& ms) const
{
    SDL_Event e;
    bool quit = false;
    s32 keybutton = 0;

    std::memset(kb.change, 0, sizeof(kb.change));

    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {

        case SDL_QUIT:
            quit = true;
            break;

        case SDL_KEYDOWN:
            keybutton = e.key.keysym.sym;
            if (keybutton < sizeof(kb.keys))
            {
                kb.change[keybutton] = !kb.keys[keybutton];
                kb.keys[keybutton] = true;
            }
            break;

        case SDL_KEYUP:
            keybutton = e.key.keysym.sym;
            if (keybutton < sizeof(kb.keys))
            {
                kb.keys[keybutton] = false;
                kb.change[keybutton] = true;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            keybutton = e.button.button;
            if (keybutton < sizeof(ms.button)) ms.button[keybutton] = true;
            break;

        case SDL_MOUSEBUTTONUP:
            keybutton = e.button.button;
            if (keybutton < sizeof(ms.button)) ms.button[keybutton] = false;
            break;

        case SDL_MOUSEMOTION:
            ms.x  = f32(e.motion.x);
            ms.y  = f32(e.motion.y);
            ms.dx = f32(e.motion.xrel);
            ms.dy = f32(e.motion.yrel);
            break;

        }
    }

    return !quit;
}

void Window::present() const
{
    SDL_Window* window = cast_wnd(m_window);
    SDL_GL_SwapWindow(window);
}
