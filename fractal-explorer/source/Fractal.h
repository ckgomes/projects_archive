#pragma once

#include "Complex.h"
#include "Vec2.h"

#include <thread>

#define PALLETE_SIZE 1
#define color_i(x) (max_iterations * x)

// This class was designed with performance in mind
// Real time fractal rendering in the CPU tends to be awfully slow
// No matter how good your PC is or how clever you think your code is
// That being said there are many optimizations that can be made
//
// Fractals are shapes with unique properties such as
// - The are infinite, meaning you can zoom in a fractal and you will always see new shapes - in practice we're restricted to what our hardware's limits
// - Self similarity, meaning as you zoom in you will find similar instances of the original fractal, but they are akways different
// 
// More information:
// http://en.wikipedia.org/wiki/Mandelbrot_set
//
namespace Fractal
{
	typedef struct ViewPort
	{
		Ved2 P0;		// Top left of the viewport
		Ved2 P1;		// Bottom right of the viewport
		inline const double dx()
		{
			return P1.x - P0.x;
		}
		inline const double dy()
		{
			return P1.y - P0.y;
		}
	} ViewPort, *pViewPort;

	// This is pre-processed for performance
	typedef struct Normalizer
	{
		double dx;		// Difference between P1x and P0x
		double ox;		// positive(dx) / dx
		double dy;		// Difference between P1y and P0y
		double oy;		// positive(dy) / dy
	} Normalizer, *pNormalizer;

	typedef struct PixelLUT
	{
		unsigned int x; // X coordinate
		unsigned int y; // Y coordinate
	} PixelLUT, *pPixelLUT;

	typedef D3DCOLOR* Palette;

	class Fractal
	{
	public:
		// In plain english, this will affect how good the fractal looks, if you increase it you will be able to zoom in more
		const unsigned short max_iterations;
	protected:
		// Viewport / screen resolution in pixels
		unsigned int pxViewportWidth;
		unsigned int pxViewportHeight;
		unsigned int pxViewportArea;
		// Mandelbrot structures
		// Viewport
		ViewPort vp_rst;
		ViewPort vp;
		// Converts pixels to normalized mandelbrot coordinates
		Normalizer normalizer_rst;
		Normalizer normalizer;
		// Pixel lookup table (speedup!)
		pPixelLUT pxLUT;
		// Fractal texture
		D3DCOLOR* fractal;
		// This is where the pre-processed colors are stored
		Palette palette;
		// Animation
		// Pointer to current position in paletter, for animation effects
		unsigned short cursorPosition;
		bool animationState;
	public:
		// Normalizes viewport positions in mandelbrot's scale
		inline double normalizeX(const unsigned int x) const
		{
			return normalizer.dx * x / pxViewportWidth - normalizer.ox;
		}
		inline double normalizeY(const unsigned int y) const
		{
			return normalizer.dy * y / pxViewportHeight - normalizer.oy;
		}
		bool autoAnimate;
		Fractal(unsigned int vpWidth, unsigned int vpHeight, unsigned short max_iterations, D3DCOLOR* pRenderTarget, bool autoAnimate = false);
		~Fractal();

		// Increases or decreases viewport area
		// The zoom parameter specifies how fast the fractal zooms in / out (negative / positive) values for zoom
		// The region will define where to zoom in, values should be between or equal to (0 ~ 1)
		// For example (0.5, 0.5) will target the center of the window, which is what you will want most of the time
		void Scale(double zoom, Ved2 region);
		// Percentage to increment or decrement (% of the pixels to move)
		void TranslateY(double dy);
		void TranslateX(double dx);
		// This will affect performance a little, but the way it's implemented the impact should be minimal
		// If auto animate is specified everytime you draw the fractal it will call this with offset = 1
		// Otherwise you can manually call it and set auto animate to false
		void AnimateColor(unsigned short offset);
		// Renders the fractal to the draw target
		virtual void DrawFractal(unsigned char nThreads) = 0;
		void Reset();
	};
};