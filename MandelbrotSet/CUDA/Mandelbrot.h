#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <math.h>
#include <chrono>
#include <iostream>
#include <fstream>

using namespace std;

#define WIDTH 7640
#define HEIGHT 7640

string IMG_NAME= "image-7640x7640.ppm";

#define x_min -2.5
#define x_max 1.0
#define y_min -1.0
#define y_max 1.0

#define r_1 99
#define g_1 111
#define b_1 164
#define r_2 232
#define g_2 203
#define b_2 192

const bool color = 1;

#define ITERATIONS 100