#pragma once

#include "Fractal.h"

namespace Fractal
{
	class Mandelbrot : public Fractal
	{
	public:
		Mandelbrot(unsigned int vpWidth, unsigned int vpHeight, unsigned short max_iterations, D3DCOLOR* pRenderTarget, Palette palette, bool autoAnimate = false) : Fractal(vpWidth, vpHeight, max_iterations, pRenderTarget, autoAnimate)
		{
			// This is the region in the complex plane where everything happens
			vp.P0.x = -2.5;
			vp.P1.x = +1.0;
			vp.P0.y = -1.0;
			vp.P1.y = +1.0;
			// Coordinate normalization
			normalizer.dx = vp.P1.x - vp.P0.x;
			normalizer.dy = vp.P1.y - vp.P0.y;
			// dx * magic number = offset for initial viewport in x axis
			normalizer.ox = normalizer.dx * 0.7142857142857142;
			// dy * magic number = offset for initial viewport in y axis
			normalizer.oy = normalizer.dy * 0.5000000000000000;

			// Color palette is created here - it's possible to change the contents of this loop to get
			this->palette = palette;

			// This allows you to reset the fractal
			vp_rst = vp;
			normalizer_rst = normalizer;
		}

		void DrawFractal(unsigned char nThreads) override
		{
			// Now for the iterations
			double area = pxViewportArea / nThreads;
			std::thread *pThreads = new std::thread[nThreads];

			for (unsigned char t = 0; t < nThreads; t++)
				// Lambda functions FTW
				pThreads[t] = std::thread([=]() {
				for (unsigned int pixel = round(area * t); pixel < round((area) * (t + 1)); pixel++)
				{
					Complex Z(0, 0);
					Complex C(normalizeX(pxLUT[pixel].x), normalizeY(pxLUT[pixel].y));

					unsigned short iterations = 0;
					// When the absolute value goes beyond 3.5 the number will diverge at that point
					while (iterations++ < max_iterations && Z.abs() <= 3.5) Z = Z.sq() + C;
					// Drawing to pixel from the color palette, relative to the current position of the cursor
					// TODO: test if performance increases
					//fractal[pixel] = palette[iterations]; // 48.16ms
					fractal[pixel] = *(palette + ((cursorPosition + iterations) % color_i(PALLETE_SIZE))); // 47.54ms
				}
			});

			for (unsigned char t = 0; t < nThreads; t++)
				pThreads[t].join();

			if (autoAnimate) AnimateColor(1);
		}
	};
};