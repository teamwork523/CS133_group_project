#ifndef G_BLUR_H
#define G_BLUR_H

#include <omp.h>
#include <stdio.h>
#include <math.h>
#include "IO/bmpfile.h"

int gaussian_blur (char* in_image_path, double sigma){
}

int gaussian_blur_parallel (char* in_image_path, double sigma, int num_of_threads, int chunk_size){
}

#endif
