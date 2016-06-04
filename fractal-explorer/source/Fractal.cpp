#include "Fractal.h"
#include <assert.h>

Fractal::Fractal::Fractal(unsigned int vpWidth, unsigned int vpHeight, unsigned short max_iterations, D3DCOLOR* pRenderTarget, bool autoAnimate) : max_iterations(max_iterations), animationState(true)
{
	// Initialize fields
	fractal = pRenderTarget;
	this->autoAnimate = autoAnimate;
	cursorPosition = 0;
	// Initial viewport
	pxViewportWidth = vpWidth;
	pxViewportHeight = vpHeight;
	pxViewportArea = vpWidth * vpHeight;

	// Pixel LUT
	pxLUT = new PixelLUT[vpWidth * vpHeight];
	for (unsigned int pixel = 0; pixel < vpWidth * vpHeight; pixel++)
	{
		pxLUT[pixel].x = pixel % vpWidth;
		pxLUT[pixel].y = pixel / vpWidth;
	}
}

Fractal::Fractal::~Fractal()
{
	if (pxLUT != nullptr)
	{
		delete[] pxLUT;
		pxLUT = nullptr;
	}
}

void Fractal::Fractal::Scale(double zoom, Ved2 region)
{
	// When we zoom in we are chaging the size of our viewport
	// In practice that means the fractal will move (translate)
	// In order to keep the fractal centered we need to compensate for that
	// By doing the inverse translation (that's the simplest way to do it anyway)
	TranslateX(region.x * zoom);
	TranslateY(region.y * zoom);
	// Here we are doing the actual zoom
	normalizer.dx += normalizer.dx * zoom;
	normalizer.dy += normalizer.dy * zoom;
}

void Fractal::Fractal::TranslateY(double dy)
{
	normalizer.oy += normalizer.dy * dy;
}

void Fractal::Fractal::TranslateX(double dx)
{
	normalizer.ox += normalizer.dx * dx;
}

void Fractal::Fractal::AnimateColor(unsigned short offset)
{
	animationState ? cursorPosition + offset < color_i(PALLETE_SIZE) - 1 ? cursorPosition += offset : animationState = !animationState :
		cursorPosition - offset >= 0 ? cursorPosition-=offset : animationState = !animationState;
}

void Fractal::Fractal::Reset()
{
	vp = vp_rst;
	normalizer = normalizer_rst;
}
