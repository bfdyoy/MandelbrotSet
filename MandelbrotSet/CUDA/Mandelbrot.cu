#include "Mandelbrot.h"

class gpu_complex {
private:

	double im;
	double re;

public:
	
	__device__ gpu_complex()
	{
		re = 0;
		im = 0;
	}

	__device__ gpu_complex(double real, double imaginary)
	{
		re = real;
		im = imaginary;
	}

	__device__ double real_squared()
	{
		return re * re;
	}

	__device__ double imaginary_squared()
	{
		return im * im;
	}

	__device__ double get_modulo()
	{
		return (real_squared() + imaginary_squared());
	}

	__device__ gpu_complex operator* (const gpu_complex& a)
	{
		return gpu_complex(re * a.re - im * a.im, re * im + im * a.re);
	}

	__device__ gpu_complex operator+ (const gpu_complex & a)
	{
		return gpu_complex(re + a.re, im + a.im);
	}
};

__device__ int check_iterations(int x, int y, const bool color)
{
	double s_x = (double)x / WIDTH - 1.5;
	double s_y = (double)y / HEIGHT - 0.5;
	gpu_complex z;
	gpu_complex point(s_x, s_y);
	unsigned int number_iterations = 0;
	while ((z.get_modulo() < 2) && (number_iterations <= 80))
	{
		z = z * z + point;
		number_iterations++;
	}
	if (number_iterations < 256)
	{
		return (255 * number_iterations) / 256;
	}
	return 255;
}

__global__ void calculate_mono(int* img)
{
	int x = blockIdx.x;
	int y = blockIdx.y;
	int tid = x + y * gridDim.x;

	img[tid] = check_iterations(x, y, 0);
}

void calculateMandelbrot(int* image) {
    dim3 blocksPerGrid(WIDTH, HEIGHT);
    int* dev_out;
    std::chrono::steady_clock::time_point start;
    size_t memSize;
    
	memSize = WIDTH * HEIGHT * sizeof(int);

    cudaMalloc(&dev_out, memSize);
    cudaMemcpy(dev_out, image, memSize, cudaMemcpyHostToDevice);
    start = std::chrono::high_resolution_clock::now();
    
	calculate_mono << <blocksPerGrid, 1 >> > (dev_out);
   
    cudaMemcpy(image, dev_out, memSize, cudaMemcpyDeviceToHost);

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    cudaFree(dev_out);
}


int main()
{

	size_t memorySize = WIDTH * HEIGHT * sizeof(int);
	int* image = (int*)malloc(memorySize);
	if (image == NULL)
		return -1;
	memset(image, 0, memorySize);
	calculateMandelbrot(image);
	ofstream img(IMG_NAME);
	if (img.is_open())
	{
		img << "P3\n" << WIDTH << " " << HEIGHT << " 255\n";
		for (int row = 0; row < WIDTH; row++)
		{
			for (int col = 0; col < HEIGHT; col++)
			{
				img << 0 << " " << image[row * WIDTH + col] << " " << image[row * WIDTH + col] << "\n";
			}
		}
		img.close();
	}
	else
	{
		cout << "Can not open the file !\n";
		exit(1);
	}
	free(image);
	return 0;
}
