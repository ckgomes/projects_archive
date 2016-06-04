Fractal explorer is a software rendered fractal explorer program.

It renders Mandelbrot and Julia sets in real time and allows panning and zooming.

This is a CPU melter, especially if you are using many threads.

The code could still be quite optimized, but at for deep zooms it would be necessary to use 128 bit floats or even fixed point, which at the time of this writing, is not really supported by the hardware and is thus painfully slow.