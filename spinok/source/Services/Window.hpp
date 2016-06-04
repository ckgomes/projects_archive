#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "../Common/Configuration.hpp"

#include "Input.hpp"

class Window final
{

public:

    Window(const char* title, s32 width, s32 height);
    ~Window();

    Window(Window&& window);
    const Window& operator=(Window&& window);

    void setTitle(const char* title);
    void setResolution(s32 width, s32 height);

    void getResolution(f32& width, f32& height) const;
    void getResolution(s32& width, s32& height) const;

    f32 aspectRatio() const;

    bool hasGLContext() const noexcept;

    // Updates the message loop with the OS
    // Grabs input for the mouse and keyboard too
    // Returns false when the window should close
    bool update(Keyboard& kb, Mouse& ms) const;

    // Swaps buffers presenting the current framebuffer to the screen
    void present() const;

    // Disabled
    Window() = delete;
    Window(const Window&) = delete;
    const Window& operator=(const Window&) = delete;

private:

    f32 m_width;
    f32 m_height;

    void* m_window  = nullptr;
    void* m_context = nullptr;

};

#endif // WINDOW_HPP
