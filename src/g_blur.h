#ifndef G_BLUR_H
#define G_BLUR_H

#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "IO/bmpfile.h"

#define MTX_ROW 7
#define MTX_COL 7

// min/max macro
#define min(a, b)  (((a) < (b)) ? (a) : (b)) 
#define max(a, b)  (((a) > (b)) ? (a) : (b))

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
    // start timer
    clock_t start = clock();
    
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
        printf("Input image width is %d, height is %d\n", width, height);
    }
    else {
        printf("Cannot read from file %s\n", in_image_path);
        exit(1);
    }
    
    // convert the input image into grayscale
    rgb_pixel_t *pixel;
    /*double *gray_mtx = (double *)malloc(sizeof(double) * width * height);
    for (j = 0; j < height; ++j){
        for (i = 0; i < width; ++i){
            pixel = bmp_get_pixel(input_img, i, j);
            gray_mtx[j*width+i] = pixel->red*0.3 + pixel->green*0.59 + pixel->blue*0.11;
        }
    }*/
    
    // create output image
    output_img = bmp_create(width, height, 8);
    if (!output_img) {
        printf("Fail to create rotation output image!\n");
        exit(1);
    }
    
    //  apply Gaussian Blur
    rgb_pixel_t out_pixel;
    double clr_sum[4];
    //double clr_sum;
    int count;
    
    // Print the time before main loop
    clock_t checkpoint1 = clock();
    printf("It takes %.3fs before main loop\n", ((float)( checkpoint1 - start )) / CLOCKS_PER_SEC );
    
    for (j = 0; j < height; ++j){
        for (i = 0; i < width; ++i){
            // clear up the sum
            for (index = 0; index < 4; index++)
                clr_sum[index] = 0.0;
            //clr_sum = 0.0;
            
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
                    //clr_sum += gray_mtx[row*MTX_COL+col] * g_mtx[row*MTX_COL+col];
                }
            }
            
            // calculate the target pixel RGBA
            out_pixel.red   = clr_sum[0];
            out_pixel.green = clr_sum[1];
            out_pixel.blue  = clr_sum[2];
            out_pixel.alpha = clr_sum[3];
            /*out_pixel.red   = max(0, min(255, clr_sum));
            out_pixel.green = max(0, min(255, clr_sum));
            out_pixel.blue  = max(0, min(255, clr_sum));
            out_pixel.alpha = 255;*/
            pixel = bmp_get_pixel(input_img, i, j);
            //printf("Previous R is %d, G is %d, B is %d, A is %d\n", pixel->red, pixel->green, pixel->blue, pixel->alpha);
            //printf("Current R is %d, G is %d, B is %d, A is %d\n", out_pixel.red, out_pixel.green, out_pixel.blue, out_pixel.alpha);
            bmp_set_pixel(output_img, i, j, out_pixel);
        }
    }
    
    // Print the time after main loop
    clock_t checkpoint2 = clock();
    printf("It takes %.3fs right after main loop\n", ((float)( checkpoint2 - start )) / CLOCKS_PER_SEC );
    printf("It takes %.3fs to finish main loop\n", ((float)( checkpoint2 - checkpoint1 )) / CLOCKS_PER_SEC );
    
    // save the image
    bmp_save(output_img, "gaussian_blur_seq.bmp");
    
    bmp_destroy(input_img);
    bmp_destroy(output_img);
    
    // free dynamic memory
    if (g_mtx)      free(g_mtx);
    //if (gray_mtx)   free(gray_mtx);
    
    // Print the time after save image
    clock_t checkpoint3 = clock();
    printf("It takes %.3fs right after saving image to local disk\n", ((float)( checkpoint3 - start )) / CLOCKS_PER_SEC );
    printf("It takes %.3fs to finish saving image\n", ((float)( checkpoint3 - checkpoint2 )) / CLOCKS_PER_SEC );
    printf("The ratio between saving and loop is %f : %d\n",  ((float)( checkpoint3 - checkpoint2 ))/( checkpoint2 - checkpoint1 ), 1 );
    
    return 0;
}

int gaussian_blur_parallel (char* in_image_path, double sigma, int num_of_threads, int chunk_size){
    struct timespec start, checkpoint1, checkpoint2, checkpoint3;
    // start timer
    if (clock_gettime( CLOCK_REALTIME, &start) == -1){
        printf("Fail to fetch start time\n");
        exit(1);
    }
    //printf("Start time is %.3f\n", (float)start.tv_sec);
    
    bmpfile_t *input_img = NULL, *output_img = NULL;
    int width = 0, height = 0;
    
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
        printf("Input image width is %d, height is %d\n", width, height);
    }
    else {
        printf("Cannot read from file %s\n", in_image_path);
        exit(1);
    }
    
    // convert the input image into grayscale
    /*double *gray_mtx = (double *)malloc(sizeof(double) * width * height);
    for (j = 0; j < height; ++j){
        for (i = 0; i < width; ++i){
            pixel = bmp_get_pixel(input_img, i, j);
            gray_mtx[j*width+i] = pixel->red*0.3 + pixel->green*0.59 + pixel->blue*0.11;
        }
    }*/
    
    // create output image
    output_img = bmp_create(width, height, 8);
    if (!output_img) {
        printf("Fail to create rotation output image!\n");
        exit(1);
    }
    
    // Print the time before main loop
    if (clock_gettime( CLOCK_REALTIME, &checkpoint1) == -1){
        printf("Fail to fetch first checkpoint time\n");
        exit(1);
    }
    //printf("Checkpoint 1 time is %.3f\n", (float)checkpoint1.tv_sec);
    printf("It takes %.3fs before main loop\n", ((float)( checkpoint1.tv_sec - start.tv_sec )) / CLOCKS_PER_SEC );
    
    //  apply Gaussian Blur
    // set the number of thread
    omp_set_num_threads(num_of_threads);
    
    #pragma omp parallel shared(output_img, g_mtx)
    {
        int i,j,row,col,index;
        double clr_sum[4];
        rgb_pixel_t *pixel;
        rgb_pixel_t out_pixel;
        
        #pragma omp for schedule(dynamic, chunk_size) nowait
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
                pixel = bmp_get_pixel(input_img, i, j);
                bmp_set_pixel(output_img, i, j, out_pixel);
            }
        }
    }
    
    // Print the time after main loop
    if (clock_gettime( CLOCK_REALTIME, &checkpoint2) == -1){
        printf("Fail to fetch the second checkpoint time\n");
        exit(1);
    }
    //printf("Checkpoint 2 time is %.3f\n", (float)checkpoint2.tv_sec);
    printf("It takes %.3fs right after main loop\n", ((float)( checkpoint2.tv_sec - start.tv_sec )) / CLOCKS_PER_SEC );
    printf("It takes %.3fs to finish main loop\n", ((float)( checkpoint2.tv_sec - checkpoint1.tv_sec )) / CLOCKS_PER_SEC );
    
    // save the image
    bmp_save(output_img, "gaussian_blur_parallel.bmp");
    
    bmp_destroy(input_img);
    bmp_destroy(output_img);
    
    // free dynamic memory
    if (g_mtx)      free(g_mtx);
    
    // Print the time after save image
    if (clock_gettime( CLOCK_REALTIME, &checkpoint3) == -1){
        printf("Fail to fetch the third checkpoint time\n");
        exit(1);
    }
    //printf("Checkpoint 3 time is %.3f\n", (float)checkpoint3.tv_sec);
    printf("It takes %.3fs right after saving image to local disk\n", ((float)( checkpoint3.tv_sec - start.tv_sec )) / CLOCKS_PER_SEC );
    printf("It takes %.3fs to finish saving image\n", ((float)( checkpoint3.tv_sec - checkpoint2.tv_sec )) / CLOCKS_PER_SEC );
    printf("The ratio between saving and loop is %f : %d\n",  ((float)( checkpoint3.tv_sec - checkpoint2.tv_sec ))/( checkpoint2.tv_sec - checkpoint1.tv_sec ), 1 );
    
    return 0;
}

#endif
