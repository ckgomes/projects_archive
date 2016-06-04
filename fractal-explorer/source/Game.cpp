// How fast to zoom in / out
static double scale_k = 0.25;
// How fast to move around
static double translate_speed = 0.01;
// More iterations = more zoom in = slower
static unsigned short max_iterations = 64;
// Animation
static bool animate_locked = false;
// Benchmark
static Timer timer;
// Doesn't do anything while paused
static bool paused = true;
static bool pause_locked = false;
// Threads -- once you find how many threads work out best for you, you can put it here
static unsigned char thread_count = 16;
static bool thread_count_lockedup = false;
static bool thread_count_lockeddn = false;

static bool mouse_lock = false;
static bool mouse_locked = false;

static bool draw_julia = false;
static bool julia_locked = false;

Game::Game(HWND hWnd, KeyboardServer& kServer, const MouseServer& mServer)
: gfx(hWnd), audio(hWnd), kbd(kServer), mouse(mServer)
{
	Palette palette = new D3DCOLOR[max_iterations];
	for (unsigned short i = 0; i < max_iterations; i++)
	{
		unsigned char g = 155 * sin(3.14*i / 180.0f);
		unsigned char r = 95 * exp(-pow((i / 30 - 5), 2) / 2);
		unsigned char b = 255 * i / (float) max_iterations;

		palette[i] = D3DCOLOR((b) +(i << r) + (i << g));
	}

	mandelbrot = new Mandelbrot(gfx.SCREENWIDTH, gfx.SCREENHEIGHT, max_iterations, gfx.pSysBuffer, palette, false);
	julia = new Julia(gfx.SCREENWIDTH, gfx.SCREENHEIGHT, max_iterations, gfx.pSysBuffer, palette, false);

	std::cout << "\n\nWelcome to FractalX\n\nNotes:\n1 - Performance will probably be horrible no matter how good your computer is\n2 - You can edit the color palette and the animation style\n3 - If your system is 64 bit, you should build to the x64 platform\n4 - Be sure to set to Release otherwise the lag will be unbearable\n\nUser controls:\n[Scaling]\nQ - Zoom out\nE - Zoom in\n[Movement]\nW - Move up\nA - Move left\nS - Move down\nD - Move right\n[Animation]\nZ - Toggle animate\n[Threads]\nT - Increase thread count\nY - Decrease thread count\n[General]\nP - Toggle pause\nJ - Toggle Julia / Mandelbrot\nR - Reset model\n\nFocus the application window and press P to start / pause, have fun! >;)\n";
}

Game::~Game()
{
	if (julia != nullptr)
	{
		delete julia;
		julia = nullptr;
	}
	if (mandelbrot != nullptr)
	{
		delete mandelbrot;
		mandelbrot = nullptr;
	}
}

void Game::UpdateAndRender()
{
	if (paused)
	{
		if (kbd.KeyIsPressed('P') && !pause_locked)
		{
			paused = false;
			pause_locked = true;
			std::cout << "Unpaused\n";
		}
		else if (!kbd.KeyIsPressed('P'))
			pause_locked = false;
	}
	else
	{
		if (kbd.KeyIsPressed('P') && !pause_locked)
		{
			paused = true;
			pause_locked = true;
			std::cout << "Paused\n";
		}
		else if (!kbd.KeyIsPressed('P'))
			pause_locked = false;

		UpdateModel();
		gfx.BeginFrame();
		ComposeFrame();
		gfx.EndFrame();
	}
}

void Game::UpdateModel()
{
	// Handle user input
	
	// Fractal to draw
	if (kbd.KeyIsPressed('J') && !julia_locked)
	{
		draw_julia = !draw_julia;
		julia_locked = true;
	}
	else if (!kbd.KeyIsPressed('J'))
		julia_locked = false;

	// Threads
	if (kbd.KeyIsPressed('T') && !thread_count_lockedup)
	{
		thread_count++;
		thread_count_lockedup = true;
		std::cout << "Thread count increased\n";
	}
	else if (!kbd.KeyIsPressed('T'))
		thread_count_lockedup = false;

	if (kbd.KeyIsPressed('Y') && !thread_count_lockeddn && thread_count > 1)
	{
		thread_count--;
		thread_count_lockeddn = true;
		std::cout << "Thread count decreased\n";
	}
	else if (!kbd.KeyIsPressed('Y'))
		thread_count_lockeddn = false;

	// Zoom in
	if (kbd.KeyIsPressed('Q'))
		draw_julia ? julia->Scale(scale_k, Ved2(0.5, 0.5)) : mandelbrot->Scale(scale_k, Ved2(0.5, 0.5));
	else if (kbd.KeyIsPressed('E'))
		draw_julia ? julia->Scale(-scale_k, Ved2(0.5, 0.5)) : mandelbrot->Scale(-scale_k, Ved2(0.5, 0.5));

	// Translate viewport position
	if (kbd.KeyIsPressed('W'))
		draw_julia ? julia->TranslateY(translate_speed) : mandelbrot->TranslateY(translate_speed);
	else if (kbd.KeyIsPressed('S'))
		draw_julia ? julia->TranslateY(-translate_speed) : mandelbrot->TranslateY(-translate_speed);
	if (kbd.KeyIsPressed('A'))
		draw_julia ? julia->TranslateX(translate_speed) : mandelbrot->TranslateX(translate_speed);
	else if (kbd.KeyIsPressed('D'))
		draw_julia ? julia->TranslateX(-translate_speed) : mandelbrot->TranslateX(-translate_speed);
	
	// Animate
	if (kbd.KeyIsPressed('Z') && !animate_locked)
	{
		draw_julia ? julia->autoAnimate = !julia->autoAnimate : mandelbrot->autoAnimate = !mandelbrot->autoAnimate;
		animate_locked = true;
	}
	else if (!kbd.KeyIsPressed('Z'))
		animate_locked = false;

	// Change julia set
	if (kbd.KeyIsPressed('X') && !mouse_locked)
	{
		mouse_lock = !mouse_lock;
		mouse_locked = true;
	}
	else if (!kbd.KeyIsPressed('X'))
		mouse_locked = false;

	if (!mouse_lock && draw_julia)
		julia->C = Complex(julia->normalizeX(mouse.GetMouseX()), julia->normalizeY(mouse.GetMouseY()));

	// Reset
	if (kbd.KeyIsPressed('R'))
		draw_julia ? julia->Reset() : mandelbrot->Reset();
}

void Game::RenderFrame()
{
	// Draw mandelbrot set to render target
	draw_julia ? julia->DrawFractal(thread_count) : mandelbrot->DrawFractal(thread_count);
}