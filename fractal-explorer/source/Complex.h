#pragma once

#include <math.h>

// Z = Re + j * Im, where j = (-1)^(1/2)
class Complex
{
public:
	double real;
	double img;

	Complex() : real(0.0), img(0.0)
	{}
	Complex(double re, double im = 0.0) : real(re), img(im)
	{}
	Complex(Complex &Z) : real(Z.real), img(Z.img)
	{}
	~Complex()
	{}

	inline Complex& Complex::operator=(Complex& Z)
	{
		real = Z.real;
		img = Z.img;
		return *this;
	}

	inline Complex Complex::operator+(Complex& Z) const
	{
		return Complex(real + Z.real, img + Z.img);
	}
	inline Complex Complex::operator-(Complex& Z) const
	{
		return Complex(real - Z.real, img - Z.img);
	}
	inline Complex Complex::operator*(Complex& Z) const
	{
		return Complex(real * Z.real - img * Z.img, real * Z.img + img * Z.real);
	}
	inline Complex Complex::operator/(Complex& Z) const
	{
		double denominator = Z.real * Z.real + Z.img * Z.img;
		return Complex((real * Z.real + img * Z.img) / denominator, (img * Z.real - real * Z.img) / denominator);
	}

	inline void Complex::operator+=(const Complex& Z)
	{
		real += Z.real;
		img += Z.img;
	}

	inline void Complex::operator-=(const Complex& Z)
	{
		real -= Z.real;
		img -= Z.img;
	}

	inline void Complex::operator*=(const Complex& Z)
	{
		real = real * Z.real - img * Z.img;
		img = real * Z.img + img * Z.real;
	}

	inline void Complex::operator/=(const Complex& Z)
	{
		double denominator = Z.real * Z.real + Z.img * Z.img;
		real = (real * Z.real + img * Z.img) / denominator;
		img = (img * Z.real - real * Z.img) / denominator;
	}

	inline bool Complex::operator==(const Complex& Z) const
	{
		return real == Z.real && img == Z.img;
	}

	inline bool Complex::operator!=(const Complex& Z) const
	{
		return real != Z.real || img != Z.img;
	}

	inline Complex Complex::conjugate(void) const
	{
		return Complex(real, -img);
	}

	inline Complex Complex::sq(void) const
	{
		return Complex(real * real - img * img, 2 * real * img);
	}

	inline double Complex::abs(void) const
	{
		return sqrt(real * real + img * img);
	}

	inline double Complex::arg(void) const
	{
		return atan(img / real);
	}
	// TODO: Implement more math
};