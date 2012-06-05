#ifndef G_BLUR_H
#define G_BLUR_H

#include <omp.h>
#include <stdio.h>
#include <math.h>
#include "IO/bmpfile.h"

double gaussian_func (int x, int y, double sigma){
    const double PI = atan(1.0)*4;
    return 1.0/(2*PI*sigma*sigma)*exp(-(x*x+y*y)/(2.0*sigma*sigma));
}

int gaussian_blur (char* in_image_path, double sigma){
    bmpfile_t *input_img = NULL, *output_img = NULL;
    int width = 0, height = 0, i, j;
    
    // read from file
    input_img = bmp_create_8bpp_from_file(in_image_path);
    
    if (input_img){
        width = bmp_get_width(input_img);
        height = bmp_get_height(input_img);
    }
    else {
        printf("Cannot read from file %s\n", in_image_path);
        exit(1);
    }
    
    // create output image
    output_img = bmp_create(width, height, 8);
    if (!output_img) {
        printf("Fail to create rotation output image!\n");
        exit(1);
    }
    
    //  apply Gaussian Blur
    for (j = 0; j < height; ++j){
        for (i = 0; i < width; ++i){
            double g_val = gaussian_func(i-width/2, j-height/2, sigma);
            rgb_pixel_t *pixel = bmp_get_pixel(input_img, i, j);
            pixel->red *= g_val;
            pixel->green *= g_val;
            pixel->blue *= g_val;
            pixel->alpha *= g_val;
            bmp_set_pixel(output_img, i, j, *pixel);
        }
    }
    
    // save the image
    bmp_save(output_img, "gaussian_blur_seq.bmp");
    
    bmp_destroy(input_img);
    bmp_destroy(output_img);
    
    return 0;
}

int gaussian_blur_parallel (char* in_image_path, double sigma, int num_of_threads, int chunk_size){
}

#endif
