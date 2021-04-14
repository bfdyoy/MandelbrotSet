#include <iostream>
#include <mpi.h>
#include <fstream>
#include <stdlib.h>
#include <complex>
#include <chrono>


using namespace std::chrono;
using namespace std;

#define DATA_TAG 1
#define TERMINATE_TAG 0
#define RESULT_TAG 2

const int width = 7640;
const int height = 7640;
#define IMG_NAME "image-7640x7640_10p.PPM"

void master();
void slave();
int cal_pixel(int x, int y, int image_width, int image_height);

int coords[width][height];

int main(int argc, char* argv[]) 
{

	int rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) 
	{
		master();
	}
	else 
	{
		slave();
	}	

	MPI_Finalize();
	return 0;
}

void master() 
{ 
	int size;
	MPI_Status status;

	int row = 0;

	int row_values[width + 1];

	ofstream fout;
	fout.open(IMG_NAME);
	fout << "P2\n";
	fout << width << " " << height << "\n";
	fout << "256\n";

	auto start = high_resolution_clock::now();

	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// send row number to all available processors (don't include master!)
	for (int i = 1; i < size; i++) 
	{
		MPI_Send(&row, 1, MPI_INT, i, DATA_TAG, MPI_COMM_WORLD);
		row++;
	}

	int doneRows = 0;
	// while slaves idle, keep sending work
	while (doneRows < height) 
	{
		MPI_Recv(&row_values, width + 1, MPI_INT, MPI_ANY_SOURCE, RESULT_TAG, MPI_COMM_WORLD, &status);

		int doneSlave = status.MPI_SOURCE;
		int receivedRow = row_values[0];

		// set the row which has finished
		for (int col = 0; col < width; col++) 
		{
			coords[receivedRow][col] = row_values[col + 1];
		}

		// increment row to send the next one
		doneRows++;
		if (row < height) 
		{

			// now we can send more data to whichever slave just finished
			MPI_Send(&row, 1, MPI_INT, doneSlave, DATA_TAG, MPI_COMM_WORLD);
			row++;
		}

	}

	for (int i = 1; i < size; i++) 
	{
		MPI_Send(0, 0, MPI_INT, i, TERMINATE_TAG, MPI_COMM_WORLD);
	}
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);

	cout << height << "x" << width << " with " << size << " processors took " << (double)duration.count() / 1000 << endl;

	for (int x = 0; x < width; x++) 
	{
		for (int y = 0; y < height; y++) {

			fout << coords[x][y] << " ";
		}
		fout << endl;
	}

	fout.close();
}

void slave() 
{

	complex<float> c;
	int slave_colors[width + 1];
	int row = 0;
	MPI_Status status;
	int slave_rank;

	MPI_Comm_rank(MPI_COMM_WORLD, &slave_rank);

	MPI_Recv(&row, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	while (status.MPI_TAG == DATA_TAG) 
	{

		if (status.MPI_TAG == TERMINATE_TAG) 
		{
			exit(0);
		}

		slave_colors[0] = row;

		// do row calculation

		for (int x = 0; x < width; x++)
		{
			slave_colors[x + 1] = cal_pixel(row, x, width, height);
		}

		//send the colors and the row back to master
		MPI_Send(slave_colors, width + 1, MPI_INT, 0, RESULT_TAG, MPI_COMM_WORLD);
		MPI_Recv(&row, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	}
}

int cal_pixel(int x, int y, int image_width, int image_height) {

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