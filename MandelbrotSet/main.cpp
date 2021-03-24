#include "cpu_mandelbrot.h"
#include <chrono>

using namespace std::chrono;

int main()
{
	auto start = high_resolution_clock::now();
	
	CPU_Mandelbrot cpu_set(7640, 7640);
	cpu_set.print_plain_fractal("image-7640x7640.ppm");
	
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);

	cout << (double) duration.count() / 1000;

	return 0;
}