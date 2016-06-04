#include "../game/platform.h"

#include <windows.h>
#include <gl/gl.h>

//////////////////////////// Game DLL entry points ////////////////////////////

typedef void (*G_initialize_t)(p_game_t*);
typedef void (*G_quit_t)(p_game_t*);
typedef void (*G_update_t)(float dt, p_timer_t*, p_input_t*, p_game_t*);
typedef void (*G_render_t)(p_framebuffer_t*, p_input_t*, p_game_t*);

G_initialize_t G_initialize = 0;
G_update_t G_update = 0;
G_render_t G_render = 0;
G_quit_t G_quit = 0;

///////////////////////////////////////////////////////////////////////////////

// Context attributes
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
// Context flags attributes
#define WGL_CONTEXT_DEBUG_BIT_ARB 0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002
// Context profile mask
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x0001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x0002

#define GL_MAJOR_VERSION 0x821B
#define GL_MINOR_VERSION 0x821C

typedef HGLRC
(*wglCreateContextAttribsARB_t)(HDC dc, HGLRC shared_context, const int* attributes);

wglCreateContextAttribsARB_t wglCreateContextAttribsARB = 0;

////////////////////////// Win32 specific data types //////////////////////////

typedef struct
{

    HWND handle;
    HDC windowDC;
    HBITMAP bitmap;
    BITMAPINFO bmi;

    int pixelformat;
    int majorversion;
    int minorversion;
    HGLRC contextGL;

    bool minimized;

} win32_window_t;

typedef struct
{

    p_framebuffer_t backbuffer;
    p_timer_t timer;
    p_input_t input;
    p_game_t game;

    win32_window_t window;
    bool quit;

} app_t;

//////////////////////// Platform independent functions ///////////////////////

void
win32_timestamp(uint64_t* timestamp)
{
    QueryPerformanceCounter((PLARGE_INTEGER) timestamp);
}

bool
win32_opengl_supports(const char* extension)
{
    return strstr(glGetString(GL_EXTENSIONS), extension) != 0;
}

bool
win32_load_asset(const char* path, uint8_t** bytes, size_t* length)
{

    const HANDLE invalid = (HANDLE) HFILE_ERROR;

    bool result = false;

    OFSTRUCT ofstruct;
    HANDLE file = (HANDLE) OpenFile(path, &ofstruct, OF_READ);

    if (file == invalid)
        return false; // ERROR: file not found, locked or no permission

    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(file, &file_size))
        goto close_return; // ERROR: unknown file size

    if (file_size.QuadPart >= 0xFFFFFFF0)
        goto close_return; // ERROR: file too big

    *bytes = VirtualAlloc(0, (SIZE_T) (file_size.QuadPart + sizeof(SIZE_T)), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (!*bytes)
        goto close_return; // ERROR: couldn't allocate memory for file

    CopyMemory(*bytes, &file_size, sizeof(SIZE_T));
    *bytes += sizeof(SIZE_T);

    result = ReadFile(file, *bytes, (DWORD) file_size.QuadPart, length, NULL);

close_return:

    CloseHandle(file);
    return result;
}

void
win32_free_asset(uint8_t* bytes)
{
    DWORD protection = PAGE_NOACCESS;
    SIZE_T length = *(SIZE_T*)(bytes - sizeof(SIZE_T));
    VirtualProtect(bytes, length, protection, &protection);
    VirtualFree(bytes, 0, MEM_RELEASE);
}

///////////////////////// Platform specific functions /////////////////////////

const DWORD mb_error = MB_OK | MB_ICONERROR;

void win32_initialize_opengl(win32_window_t* window)
{

    static const int attributes[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 2,
        WGL_CONTEXT_MINOR_VERSION_ARB, 1,
        0
    };

    PIXELFORMATDESCRIPTOR pf = {0};
    pf.nSize = sizeof(pf);
    pf.nVersion = 1;
    pf.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL;
    pf.iPixelType = PFD_TYPE_RGBA;
    pf.cColorBits = 32;

    window->pixelformat = ChoosePixelFormat(window->windowDC, &pf);
    DescribePixelFormat(window->windowDC, window->pixelformat, sizeof(pf), &pf);
    SetPixelFormat(window->windowDC, window->pixelformat, &pf);

    window->contextGL = wglCreateContext(window->windowDC);

    if (!window->contextGL)
    {
        MessageBoxA(0, "OpenGL context creationg failed", "Error", mb_error);
        return;
    }

    wglMakeCurrent(window->windowDC, window->contextGL);

    wglCreateContextAttribsARB = (wglCreateContextAttribsARB_t) wglGetProcAddress("wglCreateContextAttribsARB");
    if (!wglCreateContextAttribsARB)
    {
        MessageBoxA(0, "Could not location entry point glCreateContextAttribsARB", "Error", mb_error);
        return;
    }

    window->contextGL = wglCreateContextAttribsARB(window->windowDC, 0, attributes);

    if (!window->contextGL)
    {
        MessageBoxA(0, "OpenGL context extension failed", "Error", mb_error);
        return;
    }

    glGetIntegerv(GL_MAJOR_VERSION, &window->majorversion);
    glGetIntegerv(GL_MINOR_VERSION, &window->minorversion);

}

LRESULT CALLBACK
win32_window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{

    static app_t* app = 0;
    static PAINTSTRUCT ps = {0};

    switch (message)
    {

        case WM_CREATE:
        {

            app = ((LPCREATESTRUCT) l_param)->lpCreateParams;

            app->window.handle = window;
            app->window.windowDC = GetDC(window);
            app->window.bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            app->window.bmi.bmiHeader.biBitCount = 32;
            app->window.bmi.bmiHeader.biCompression = BI_RGB;
            app->window.bmi.bmiHeader.biPlanes = 1;

        } break;

        case WM_CLOSE:
        {
            app->quit = true;
        } break;

        case WM_SIZE:
        {

            int width = l_param & 0xFFFF;
            int height = (l_param >> 16) & 0xFFFF;
            
            if (!width || !height)
                break;
                
            if (app->window.bitmap && (width != app->backbuffer.width || height != app->backbuffer.height))
                break;

            app->backbuffer.width = width;
            app->backbuffer.height = height;

            app->window.bmi.bmiHeader.biWidth = width;
            app->window.bmi.bmiHeader.biHeight = height;

            if (app->window.bitmap)
                DeleteObject(app->window.bitmap);

            app->window.bitmap = CreateDIBSection(app->window.windowDC,
                                                    &app->window.bmi,
                                                    DIB_RGB_COLORS,
                                                    &app->backbuffer.pixels,
                                                    0, 0);

        } break;

        case WM_PAINT:
        {

            BeginPaint(window, &ps);
            SetDIBitsToDevice(
                app->window.windowDC,
                0, 0,
                app->window.bmi.bmiHeader.biWidth,
                app->window.bmi.bmiHeader.biHeight,
                0, 0, 0,
                app->window.bmi.bmiHeader.biHeight,
                app->backbuffer.pixels,
               &app->window.bmi,
                DIB_RGB_COLORS);
            EndPaint(window, &ps);

        } break;

        case WM_SYSCOMMAND:
        {

            if (w_param == SC_MINIMIZE)
                app->window.minimized = true;
            if (w_param == SC_RESTORE)
                app->window.minimized = false;

        } break;

        case WM_MOUSEMOVE:
        {
            app->input.mouse_x = l_param & 0xFFFF;
            app->input.mouse_y = app->backbuffer.height - ((l_param >> 16) & 0xFFFF) - 1; 
        } break;
        
        case WM_MOUSEWHEEL:
        {
            app->input.mouse_wheel += GET_WHEEL_DELTA_WPARAM(w_param) / WHEEL_DELTA;
        } break;
        
        case WM_LBUTTONDOWN:
        {
            app->input.mouse_transitions[P_MOUSE_LEFT] = !app->input.mouse_buttons[P_MOUSE_LEFT]; 
            app->input.mouse_buttons[P_MOUSE_LEFT] = true;
        } break;
        
        case WM_LBUTTONUP:
        {
            app->input.mouse_transitions[P_MOUSE_LEFT] = app->input.mouse_buttons[P_MOUSE_LEFT];
            app->input.mouse_buttons[P_MOUSE_LEFT] = false;
        } break;
        
        case WM_MBUTTONDOWN:
        {
            app->input.mouse_transitions[P_MOUSE_MIDDLE] = !app->input.mouse_buttons[P_MOUSE_MIDDLE];
            app->input.mouse_buttons[P_MOUSE_MIDDLE] = true;
        } break;
        
        case WM_MBUTTONUP:
        {
            app->input.mouse_transitions[P_MOUSE_MIDDLE] = app->input.mouse_buttons[P_MOUSE_MIDDLE];
            app->input.mouse_buttons[P_MOUSE_MIDDLE] = false;
        } break;
        
        case WM_RBUTTONDOWN:
        {
            app->input.mouse_transitions[P_MOUSE_RIGHT] = !app->input.mouse_buttons[P_MOUSE_RIGHT];
            app->input.mouse_buttons[P_MOUSE_RIGHT] = true;
        } break;
        
        case WM_RBUTTONUP:
        {
            app->input.mouse_transitions[P_MOUSE_RIGHT] = app->input.mouse_buttons[P_MOUSE_RIGHT];
            app->input.mouse_buttons[P_MOUSE_RIGHT] = false;
        } break;
        
    }

    return DefWindowProcA(window, message, w_param, l_param);
}

void
win32_create_window(app_t* app)
{

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = win32_window_proc;
    wc.hInstance = GetModuleHandle(0);
    wc.lpszClassName = "MODC CLASS";

    RECT rect = {0};
    rect.left = 0;
    rect.right = app->backbuffer.width;
    rect.bottom = app->backbuffer.height;
    rect.top = 0;

    DWORD window_style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    DWORD window_style_ex = WS_EX_TOPMOST;

    if (!AdjustWindowRectEx(&rect, window_style, 0, window_style_ex))
    {
        MessageBoxA(0, "Could not adjust window rectangle", "Error", mb_error);
        return;
    }

    if (!RegisterClassExA(&wc))
    {
        MessageBoxA(0, "Failed to register class", "Error", mb_error);
        return;
    }

    HWND window = CreateWindowExA(
        window_style_ex,
        wc.lpszClassName,
        "MODC",
        window_style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        0, 0,
        wc.hInstance,
        app
    );

    if (!window)
    {
        MessageBoxA(0, "Failed to create window", "Error", mb_error);
        return;
    }
}

void
win32_initialize_timer(p_timer_t* timer)
{

    int64_t now;
    QueryPerformanceFrequency((PLARGE_INTEGER) &timer->frequency);
    QueryPerformanceCounter((PLARGE_INTEGER) &now);

    for (int i = 0; i < P_TIMER_COUNT; ++i)
    {
        timer->start[i] = timer->end[i] = now;
    }

}

void
win32_load_platform_functions(p_game_t* game)
{
    // Passes the address of each platform function to the game
    game->timestamp = win32_timestamp;
    game->load_asset = win32_load_asset;
    game->free_asset = win32_free_asset;
    game->opengl_supports = win32_opengl_supports;
}

void
win32_load_game_if_needed(p_game_t* game)
{

    static FILETIME dll_update_time = {0};
    static HMODULE dll_game = 0;

    static const char* dll_file = "modc_game.dll";
    static const char* dll_temp = "modc_game_temp.dll";

    WIN32_FIND_DATA file_data;
    if (FindFirstFileA(dll_file, &file_data) == INVALID_HANDLE_VALUE)
    {
        // ERROR: cant find game DLL
        return;
    }
    if (CompareFileTime(&dll_update_time, &file_data.ftLastWriteTime) < 0)
    {

        dll_update_time = file_data.ftLastWriteTime;
        if (dll_game)
        {
            G_quit(game);
            FreeLibrary(dll_game);
        }

        CopyFileA(dll_file, dll_temp, 0);
        dll_game = LoadLibraryA(dll_temp);
        if (dll_game)
        {

            G_initialize = (G_initialize_t) GetProcAddress(dll_game, "G_initialize");
            G_update = (G_update_t) GetProcAddress(dll_game, "G_update");
            G_render = (G_render_t) GetProcAddress(dll_game, "G_render");
            G_quit = (G_quit_t) GetProcAddress(dll_game, "G_quit");

            G_initialize(game);

            MessageBeep(MB_OK);

        }
        else
        {
            // ERROR: cant load game DLL
        }
    }
}

int CALLBACK
WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{

    MSG message;
    SYSTEM_INFO system;

    uint64_t timer_now;
    app_t app = {0};

    GetSystemInfo(&system);

    app.backbuffer.width = 800;
    app.backbuffer.height = 600;

    app.game.memory_size = 1024 * 1024 * 1024;
    app.game.memory = VirtualAlloc(0, app.game.memory_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    win32_create_window(&app);
    win32_initialize_timer(&app.timer);

    win32_load_platform_functions(&app.game);
    win32_load_game_if_needed(&app.game);

    while (!app.quit)
    {

        win32_timestamp(&app.timer.start[P_TIMER_MAINLOOP]);
        
        // Clear transition states
        app.input.mouse_wheel = 0;
        ZeroMemory(app.input.mouse_transitions, sizeof(app.input.mouse_transitions));
        
        // Processing all operating system messages
        while (PeekMessageA(&message, app.window.handle, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }

        // Getting the time between each frame
        win32_timestamp(&timer_now);
        timer_now = 1000000 * (timer_now - app.timer.start[P_TIMER_GAMEUPDATE]) / app.timer.frequency;

        // Update the game
        win32_timestamp(&app.timer.start[P_TIMER_GAMEUPDATE]);
        G_update(timer_now / 1E6, &app.timer, &app.input, &app.game);
        win32_timestamp(&app.timer.end[P_TIMER_GAMEUPDATE]);

        if (!app.window.minimized)
        {

            // Render the game
            win32_timestamp(&app.timer.start[P_TIMER_GAMERENDER]);
            G_render(&app.backbuffer, &app.input, &app.game);
            win32_timestamp(&app.timer.end[P_TIMER_GAMERENDER]);

            // Display the updated state
            win32_timestamp(&app.timer.start[P_TIMER_BLIT]);
            InvalidateRect(app.window.handle, 0, 1);
            UpdateWindow(app.window.handle);
            win32_timestamp(&app.timer.end[P_TIMER_BLIT]);
        }

        win32_timestamp(&app.timer.end[P_TIMER_MAINLOOP]);

        // Use surplus time to reload the DLL, if needed
        win32_timestamp(&app.timer.start[P_TIMER_GAMELOAD]);
        win32_load_game_if_needed(&app.game);
        win32_timestamp(&app.timer.end[P_TIMER_GAMELOAD]);

        // Sleeping if the frame finished too fast (there should be a better way of doing this maybe vsync)
        int ms = 1000 * (app.timer.end[P_TIMER_MAINLOOP] - app.timer.start[P_TIMER_MAINLOOP]) / app.timer.frequency;
        if (ms < 15) Sleep(16 - ms);

    }

    G_quit(&app.game);
    PostQuitMessage(0);

    return 0;
}

