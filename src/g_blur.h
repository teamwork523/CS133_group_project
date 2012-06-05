#ifndef G_BLUR_H
#define G_BLUR_H

#include <omp.h>
#include <stdio.h>
#include <math.h>
#include "IO/bmpfile.h"

#define MTX_ROW 3
#define MTX_COL 3

double gaussian_func (int x, int y, double sigma){
    const double PI = atan(1.0)*4;
    return 1.0/(2*PI*sigma*sigma)*exp(-((x*x+y*y)*1.0/(2.0*sigma*sigma)));
}

// generate a 3*3 kernal matrix based on sigma
double* gen_gaussian_kernal (double* g_matrix, double sigma){
    int i,j;
    for (j = 0; j < MTX_ROW; ++j){
        for (i = 0; i < MTX_COL; ++i){
            g_matrix[j*MTX_COL+i] = gaussian_func(i-MTX_COL/2, j-MTX_ROW/2, sigma);
        }
    }
    return g_matrix;
}

int gaussian_blur (char* in_image_path, double sigma){
    bmpfile_t *input_img = NULL, *output_img = NULL;
    int width = 0, height = 0, i, j, row, col, index;
    
    // create Gaussian Kernal 3*3 matrix
    double *g_mtx = (double *)malloc(sizeof(double) * MTX_COL * MTX_ROW);
    g_mtx = gen_gaussian_kernal(g_mtx, sigma);
    
    // output the matrix
    /*for (row = 0; row < MTX_ROW; row++){
        for (col = 0; col < MTX_COL; col++){
            printf("%lf ", g_mtx[row*MTX_COL+col]);
        }
        printf("\n");
    }*/
    
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
    rgb_pixel_t *pixel;
    rgb_pixel_t out_pixel;
    double clr_sum[4];      // 0: Red, 1: Green, 2: Blue, 3: Alpha
    int count;
    
    for (j = 0; j < height; ++j){
        for (i = 0; i < width; ++i){
            // clear up the sum
            for (index = 0; index < 4; index++)
                clr_sum[index] = 0.0;
            
            // Blur the pixel by averaging the eight neighbour pixels
            for (row = 0; row < MTX_ROW; row++){
                for (col = 0; col < MTX_COL; col++){
                    int row_index = row+j-MTX_ROW/2, col_index = col+i-MTX_COL/2;
                    if (row_index < 0 || row_index >= height 
                        || col_index < 0 || col_index >= width)
                        continue;
                    pixel = bmp_get_pixel(input_img, col_index, row_index);
                    clr_sum[0] += pixel->red * g_mtx[row*MTX_COL+col];
                    clr_sum[1] += pixel->green * g_mtx[row*MTX_COL+col];
                    clr_sum[2] += pixel->blue * g_mtx[row*MTX_COL+col];
                    clr_sum[3] += pixel->alpha * g_mtx[row*MTX_COL+col];
                }
            }
            
            // calculate the target pixel RGBA
            out_pixel.red   = clr_sum[0];
            out_pixel.green = clr_sum[1];
            out_pixel.blue  = clr_sum[2];
            out_pixel.alpha = clr_sum[3];
            //pixel = bmp_get_pixel(input_img, i, j);
            //printf("Previous R is %d, G is %d, B is %d, A is %d\n", pixel->red, pixel->green, pixel->blue, pixel->alpha);
            //printf("Current R is %d, G is %d, B is %d, A is %d\n", out_pixel.red, out_pixel.green, out_pixel.blue, out_pixel.alpha);
            bmp_set_pixel(output_img, i, j, out_pixel);
        }
    }
    
    // save the image
    bmp_save(output_img, "gaussian_blur_seq.bmp");
    
    bmp_destroy(input_img);
    bmp_destroy(output_img);
    
    // free dynamic memory
    if (g_mtx)  free(g_mtx);
    
    return 0;
}

int gaussian_blur_parallel (char* in_image_path, double sigma, int num_of_threads, int chunk_size){
}

#endif
