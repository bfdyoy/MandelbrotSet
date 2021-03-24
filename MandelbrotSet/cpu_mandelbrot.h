#ifndef CPU_MANDELBROT_H
#define CPU_MANDELBROT_H

#include <iostream>
#include <fstream>
#include <complex>

using namespace std;

class CPU_Mandelbrot
{
private:
	int max_iterations;
	int coloring_type;
	float image_width, image_height;
	int* coloring;
public:

	CPU_Mandelbrot(float image_width, float image_height);
	void set_max_iterations(int max_it);
	void set_coloring_type(int coloring);
	int calculate_value(int x, int y);

	void print_plain_fractal(string img_name);
};

#endif