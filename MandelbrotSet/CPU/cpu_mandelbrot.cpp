#include "cpu_mandelbrot.h"

CPU_Mandelbrot::CPU_Mandelbrot(float width, float height)
{
	this->image_height = height;
	this->image_width = width;
}

int CPU_Mandelbrot::calculate_value(int x, int y)
{
	// divide the point by the image dimension so 0 < {x,y} < 1 and apply a translation
	complex<float> point((float)x / image_width - 1.5, (float)y / image_height - 0.5);

	complex<float> z(0, 0);
	unsigned int number_iterations = 0;

	while ((abs(z) < 2) && (number_iterations <= 80))
	{
		z = z * z + point;
		number_iterations++;
	}

	if (number_iterations < 256)
		return (255 * number_iterations) / 256;
	return 255;
}

void CPU_Mandelbrot::print_plain_fractal(string img_name)
{
	ofstream image(img_name);
	if (image.is_open())
	{ 
		image << "P3\n" << image_width << " " << image_height << " 255\n";
		for (int row = 0; row < image_width; row++)
		{
			for (int col = 0; col < image_height; col++)
			{
				int current_value = calculate_value(row, col);
				image << 0 << " " << current_value << " " << current_value << "\n";
			}
		}
		image.close();
	}
	else
	{
		cout << "Can not open the file !\n";
		exit(1);
	}
}

void CPU_Mandelbrot::set_coloring_type(int coloring)
{
	coloring_type = coloring;
}

void CPU_Mandelbrot::set_max_iterations(int max_iter)
{
	max_iterations = max_iter;
}