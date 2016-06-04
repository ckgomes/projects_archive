
#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include <stdbool.h>

#include <stdio.h>

///////////////////////// Platform independent types //////////////////////////

// Timer allocation slots
typedef enum
{

    P_TIMER_BLIT,
    P_TIMER_GAMEUPDATE,
    P_TIMER_GAMERENDER,
    P_TIMER_MAINLOOP,
    P_TIMER_GAMELOAD,

    P_TIMER_COUNT

} p_timer_slot_t;

// Keyboard modifiers
typedef enum
{

    P_KBMOD_NUMLOCK  = (1 << 0),
    P_KBMOD_CAPSLOCK = (1 << 1),
    P_KBMOD_ALT      = (1 << 2),
    P_KBMOD_CTRL     = (1 << 3),
    P_KBMOD_SHIFT    = (1 << 4)

} p_kbmod_t;

typedef enum
{
    
    P_MOUSE_LEFT,
    P_MOUSE_MIDDLE,
    P_MOUSE_RIGHT
    
} p_mousebutton_t;

typedef struct
{

    int width;
    int height;
    uint32_t* pixels;

} p_framebuffer_t;

typedef struct
{

    int64_t frequency;
    int64_t start[P_TIMER_COUNT];
    int64_t end[P_TIMER_COUNT];

} p_timer_t;

typedef struct
{
    
    bool mouse_visible;
    bool mouse_confined;

    int mouse_x;
    int mouse_y;
    
    int mouse_dx;
    int mouse_dy;
    
    int mouse_wheel;

    int mouse_buttons[16];
    int mouse_transitions[16];

    int keyboard_keys[256];
    int keyboard_transitions[256];

    p_kbmod_t modifiers;

} p_input_t;

////////////////////////// Platform helper functions //////////////////////////

inline bool
P_mouse_isdown(p_input_t* input, p_mousebutton_t button)
{
    return input->mouse_buttons[button] &&
           input->mouse_transitions[button];
}

inline bool
P_mouse_isreleased(p_input_t* input, p_mousebutton_t button)
{
    return !input->mouse_buttons[button] &&
           input->mouse_transitions[button];
}

/////////////////////// Platform independent functions ////////////////////////

typedef void
(*P_timestamp_t)(uint64_t* timestamp);

typedef bool
(*P_load_asset_t)(const char* path, uint8_t** bytes, size_t* length);

typedef void
(*P_free_asset_t)(uint8_t* bytes);

typedef bool
(*P_opengl_supports_t)(const char* extension);

/////////////////////// Platform independent callbacks ////////////////////////

typedef void
(*P_window_resize_t)(int width, int height);

/////////////////////////////// Game structure ////////////////////////////////

typedef struct
{

    P_timestamp_t timestamp;
    P_load_asset_t load_asset;
    P_free_asset_t free_asset;
    P_opengl_supports_t opengl_supports;

    size_t memory_size;
    void* memory;

} p_game_t;

#endif