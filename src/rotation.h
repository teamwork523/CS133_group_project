#ifndef ROTATION_H
#define ROTATION_H

#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "IO/bmpfile.h"

// min/max macro
#define min(a, b)  (((a) < (b)) ? (a) : (b)) 
#define max(a, b)  (((a) > (b)) ? (a) : (b))

#define BILLION 1000000000.0

struct timespec diff_timesp(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = BILLION+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

int rotation(char* in_image_path, double deg){
    // start timer
    clock_t start = clock();
    
    // callibrate on deg
    while (deg >= 360.0)   deg -= 360.0;
    while (deg < 0.0)     deg += 360.0;
    
    // Our image coordiante is flipped by x-axis and assume deg is counterclockwise
    // the actual deg it rotate in Cartesian Coordinate is 360 - deg
    deg = 360.0 - deg;
    
    bmpfile_t *input_img = NULL, *output_img = NULL;
    int in_width = 0, in_height = 0, out_width = 0, out_height = 0;
    double deg_radian = 0.0;
    // define pi
    const double PI = atan(1.0)*4;
    deg_radian = deg*PI/180.0;
    
    // calculate the sin/cos of given degree
    double sine = 0.0, cosine = 0.0;
    sine = sin(deg_radian);
    cosine = cos(deg_radian);
    
    // read input image
    input_img = bmp_create_8bpp_from_file(in_image_path);
    //bmp_save(input_img, "bmp_rotation_read.bmp");

    if (input_img){
        in_width = bmp_get_width(input_img);
        in_height = bmp_get_height(input_img);
    }
    else {
        printf("Cannot read from file %s\n", in_image_path);
        exit(1);
    }
 
    printf("Read image width is %d, height is %d\n", in_width, in_height);
    
    // Assume the origin to be the left-upper corner
    // First compute the other three corners
    double x1 = -in_height*sine, y1 = in_height*cosine;     // lower-left
    double x2 = in_width*cosine-in_height*sine, y2 = in_height*cosine+in_width*sine;    // lower-right
    double x3 = in_width*cosine, y3 = in_width*sine;        // upper-right
    
    // calcuate the new height and width
    double minx = min(0, min(x1, min(x2, x3)));
    double miny = min(0, min(y1, min(y2, y3)));
    double maxx = max(0, max(x1, max(x2, x3)));
    double maxy = max(0, max(y1, max(y2, y3)));
    
    out_width = (int)(abs(maxx - minx));
    out_height = (int)(abs(maxy - miny));
    
    printf("New width is %d, new height is %d\n", out_width, out_height);
    
    // apply rotation operation
    output_img = bmp_create(out_width, out_height, 8);
    if (!output_img) {
        printf("Fail to create rotation output image!\n");
        exit(1);
    }
    
    // Print the time before main loop
    clock_t end_setup = clock();
    double setup_time = end_setup - start;
    printf("Setup time is %.3lfs\n", setup_time / CLOCKS_PER_SEC );
    
    int i,j;
    for (i = 0; i < out_width; ++i){
        for (j = 0; j < out_height; ++j){
            // rotate (-deg) counterclock wise to fetch previous coordinate
            // shift i,j with minx/miny
            int src_x = (int)((i+minx)*cosine+(j+miny)*sine);
            int src_y = (int)((j+miny)*cosine-(i+minx)*sine);
            if (src_x >= 0 && src_x < in_width && src_y >= 0 && src_y < in_height){
                rgb_pixel_t *pixel = bmp_get_pixel(input_img, src_x, src_y);
                bmp_set_pixel(output_img, i, j, *pixel);
            }
        }
    }
    
    // Print the time after main loop
    clock_t end_loop = clock();
    double loop_time = end_loop - end_setup;
    printf("Loop time is %.3lfs\n", loop_time / CLOCKS_PER_SEC );
    
    bmp_save(output_img, "bmp_rotation_seq.bmp");
    
    bmp_destroy(input_img);
    bmp_destroy(output_img);
    
    // Print the time after save image
    clock_t end_save = clock();
    double save_time = end_save - end_loop;
    printf("Save time is %.3lfs\n", save_time / CLOCKS_PER_SEC );
    printf("The ratio between saving and loop is %.3lf:1\n",  save_time/loop_time );
    
    return 0;
}

int rotation_parallel(char* in_image_path, double deg, int num_of_threads, int chunk_size){
    // define time struct
    struct timespec start, end_setup, end_loop, end_save;
    clock_gettime( CLOCK_REALTIME, &start);
    
    // callibrate on deg
    while (deg >= 360.0)   deg -= 360.0;
    while (deg < 0.0)     deg += 360.0;
    
    // Our image coordiante is flipped by x-axis and assume deg is counterclockwise
    // the actual deg it rotate in Cartesian Coordinate is 360 - deg
    deg = 360.0 - deg;
    
    bmpfile_t *input_img = NULL, *output_img = NULL;
    int in_width = 0, in_height = 0, out_width = 0, out_height = 0;
    double deg_radian = 0.0;
    // define pi
    const double PI = atan(1.0)*4;
    deg_radian = deg*PI/180.0;
    
    // calculate the sin/cos of given degree
    double sine = 0.0, cosine = 0.0;
    sine = sin(deg_radian);
    cosine = cos(deg_radian);
    
    // read input image
    input_img = bmp_create_8bpp_from_file(in_image_path);
    //bmp_save(input_img, "bmp_rotation_read.bmp");

    if (input_img){
        in_width = bmp_get_width(input_img);
        in_height = bmp_get_height(input_img);
    }
    else {
        printf("Cannot read from file %s\n", in_image_path);
        exit(1);
    }
 
    printf("Read image width is %d, height is %d\n", in_width, in_height);
    
    // Assume the origin to be the left-upper corner
    // First compute the other three corners
    double x1 = -in_height*sine, y1 = in_height*cosine;     // lower-left
    double x2 = in_width*cosine-in_height*sine, y2 = in_height*cosine+in_width*sine;    // lower-right
    double x3 = in_width*cosine, y3 = in_width*sine;        // upper-right
    
    // calcuate the new height and width
    double minx = min(0, min(x1, min(x2, x3)));
    double miny = min(0, min(y1, min(y2, y3)));
    double maxx = max(0, max(x1, max(x2, x3)));
    double maxy = max(0, max(y1, max(y2, y3)));
    
    out_width = (int)(abs(maxx - minx));
    out_height = (int)(abs(maxy - miny));
    
    printf("New width is %d, new height is %d\n", out_width, out_height);
    
    // apply rotation operation
    output_img = bmp_create(out_width, out_height, 8);
    if (!output_img) {
        printf("Fail to create rotation output image!\n");
        exit(1);
    }
    
    // before loop
    clock_gettime( CLOCK_REALTIME, &end_setup);
    struct timespec diff_time = diff_timesp(start, end_setup);
    printf("Setup time is %.3lfs\n", (double)(diff_time.tv_sec+diff_time.tv_nsec/BILLION));
    
    // set the number of thread
    omp_set_num_threads(num_of_threads);
    
    #pragma omp parallel shared(out_width, out_height, minx, miny, output_img)
    {
        int i,j;
        #pragma omp for schedule(dynamic, chunk_size) nowait
        for (i = 0; i < out_width; ++i){
            for (j = 0; j < out_height; ++j){
                // rotate (-deg) counterclock wise to fetch previous coordinate
                // shift i,j with minx/miny
                int src_x = (int)((i+minx)*cosine+(j+miny)*sine);
                int src_y = (int)((j+miny)*cosine-(i+minx)*sine);
                if (src_x >= 0 && src_x < in_width && src_y >= 0 && src_y < in_height){
                    rgb_pixel_t *pixel = bmp_get_pixel(input_img, src_x, src_y);
                    bmp_set_pixel(output_img, i, j, *pixel);
                }
            }
        }
    }
    
    // after loop
    clock_gettime( CLOCK_REALTIME, &end_loop);
    struct timespec loop_time = diff_timesp(end_setup, end_loop);
    printf("Loop time is %.3lfs\n", (double)(loop_time.tv_sec+loop_time.tv_nsec/BILLION));
    
    bmp_save(output_img, "bmp_rotation_parallel.bmp");
    
    bmp_destroy(input_img);
    bmp_destroy(output_img);
    
    // after saving image
    clock_gettime( CLOCK_REALTIME, &end_save);
    struct timespec save_time = diff_timesp(end_loop, end_save);
    printf("Save time is %.3lfs\n", (double)(save_time.tv_sec+save_time.tv_nsec/BILLION));
    printf("The ratio between saving and loop is  %.3lf:1\n", ((double)(save_time.tv_sec+save_time.tv_nsec/BILLION))/((double)(loop_time.tv_sec+loop_time.tv_nsec/BILLION)));
    
    return 0;
}

#endif






