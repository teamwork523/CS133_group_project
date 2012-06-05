#ifndef MOT_EST_H
#define MOT_EST_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>
#include "IO/bmpfile.h"

int motion_estimation(char* input_img1, char* input_img2)
{
	const int BLOCK_SIZE = 16;
	bmpfile_t *bmp1 = NULL, *bmp2 = NULL, *bmp_out = NULL;
	int row,col;
	int total_loops,sad;
	int sad_min;
	int i,j,k,m,p,q,k1,m1,p1,q1;
	
	FILE * pFile;
	pFile = fopen("output.txt", "w");

	bmp1 = bmp_create_8bpp_from_file(input_img1);
	bmp2 = bmp_create_8bpp_from_file(input_img2);
	
	int width = bmp_get_width(bmp1);
	int mb_width = width / BLOCK_SIZE;
	
	int height = bmp_get_height(bmp1);
	int mb_height = height / BLOCK_SIZE;

	
	int* sad_MATRIX = malloc(sizeof(int) * mb_width * mb_height);	

	total_loops = 1;	
	sad = 0;

	rgb_pixel_t** pixels1 = malloc(sizeof(rgb_pixel_t*) * width * height);
	rgb_pixel_t** pixels2 = malloc(sizeof(rgb_pixel_t*) * width * height);
	rgb_pixel_t** pixels_out = malloc(sizeof(rgb_pixel_t*) * width * height);

	int diff = 0;
	int loop_number;



	for (loop_number = 0;  loop_number < total_loops; loop_number++)
	{

		//read from the file
		for (i = 0; i < width; ++i) 
		{
			for (j = 0; j < height; ++j) 
			{
				pixels1[j*width+i] = bmp_get_pixel(bmp1, i, j);
				pixels2[j*width+i] = bmp_get_pixel(bmp2, i, j);
			}
		}
		for(i = 0; i < mb_height; i++) 
		{  
			for(j = 0; j< mb_width; j++) 
			{
				for(p = 0; p < mb_height; p++) 
				{
					for(q = 0; q < mb_width; q++)
					{
						for(k = 0; k < BLOCK_SIZE; k++)         
						{
							for(m = 0; m < BLOCK_SIZE; m++) 
							{
								rgb_pixel_t* pixel1 = pixels1[BLOCK_SIZE*p*width+BLOCK_SIZE*q+m+k*width];
								rgb_pixel_t* pixel2 = pixels2[BLOCK_SIZE*i*width+BLOCK_SIZE*j+m+k*width];
								if(pixel1 == NULL || pixel2 == NULL)
								{
									printf("%d %d\n", p, q);
									printf("error!\n");
									return 1;
								}
								diff = abs(pixel1->blue - pixel2->blue) + abs(pixel1->green - pixel2->green) + 
								       abs(pixel1->red - pixel2->red) + abs(pixel1->alpha - pixel2->alpha);
								sad = sad + diff; 
							}		  			          
						}					
						sad_MATRIX[p*mb_width+q] = sad;				  
						sad = 0;
					}                 
				}   				
				sad_min = INT_MAX; 
				for(p1 = 0; p1 < mb_height; p1++)
				{
					for(q1 = 0; q1 < mb_width; q1++)
					{
						if(sad_MATRIX[p1*mb_width+q1] == sad_min)                            
						{
							if(abs(q1-i) + abs(p1-j) < abs(row-i) + abs(col-j))
							{
								row = p1;
								col = q1;		
							}
						}
						
						if(sad_MATRIX[p1*mb_width+q1] < sad_min)
						{
							sad_min = sad_MATRIX[p1*mb_width+q1];
							row = p1;
							col = q1;						   
						}
						
					}
				}
				for(k1 = 0; k1 < BLOCK_SIZE; k1++)                  
				{
					for(m1 = 0; m1 < BLOCK_SIZE; m1++) 
					{
						pixels_out[BLOCK_SIZE*i*width+BLOCK_SIZE*j+m1+k1*width] = pixels1[BLOCK_SIZE*row*width+BLOCK_SIZE*col+m1+k1*width];
					}		  			    
				}
				fprintf(pFile, "Block Number %d %d: [%d %d]\n", i, j, col-j, row-i);
			}		 		 		 

		}
	}
	bmp_out = bmp_create(width, height, 8);
	if (bmp_out != NULL) 
	{
		for (i = 0; i < width; i++) 
		{
			for (j = 0; j < height; j++) 
			{
				bmp_set_pixel(bmp_out, i, j, *(pixels_out[j*width+i]));
			}
		}
		bmp_save(bmp_out, "bmp_out.bmp");
	}
	  
	bmp_destroy(bmp1);
	bmp_destroy(bmp2);
	bmp_destroy(bmp_out);
	
	free(sad_MATRIX);
	free(pixels1);
	free(pixels2);
	free(pixels_out);
	fclose(pFile);

	return 0;	

}


int motion_estimation_parallel(char* input_img1, char* input_img2, int num_threads)
{

	omp_set_num_threads(num_threads);
	const int BLOCK_SIZE = 16;
	bmpfile_t *bmp1 = NULL, *bmp2 = NULL, *bmp_out = NULL;
	int row,col;
	int total_loops,sad;
	int sad_min;
	int i,j,k,m,p,q,k1,m1,p1,q1;
	
	FILE * pFile;
	pFile = fopen("output.txt", "w");

	bmp1 = bmp_create_8bpp_from_file(input_img1);
	bmp2 = bmp_create_8bpp_from_file(input_img2);
	
	int width = bmp_get_width(bmp1);
	int mb_width = width / BLOCK_SIZE;
	
	int height = bmp_get_height(bmp1);
	int mb_height = height / BLOCK_SIZE;

		

	total_loops = 1;	

	rgb_pixel_t** pixels1 = malloc(sizeof(rgb_pixel_t*) * width * height);
	rgb_pixel_t** pixels2 = malloc(sizeof(rgb_pixel_t*) * width * height);
	rgb_pixel_t** pixels_out = malloc(sizeof(rgb_pixel_t*) * width * height);

	int diff = 0;
	int loop_number;
	
	for (i = 0; i < width; ++i) 
	{
		for (j = 0; j < height; ++j) 
		{
			pixels1[j*width+i] = bmp_get_pixel(bmp1, i, j);
			pixels2[j*width+i] = bmp_get_pixel(bmp2, i, j);
		}
	}

	#pragma omp parallel private(row, col, sad, sad_min, i,j,k,m,p,q,k1,m1,p1,q1, diff)
	{
		sad = 0;
		int* sad_MATRIX = malloc(sizeof(int) * mb_width * mb_height);
		#pragma omp for
		for(i = 0; i < mb_height; i++) 
		{  
			for(j = 0; j< mb_width; j++) 
			{
				for(p = 0; p < mb_height; p++) 
				{
					for(q = 0; q < mb_width; q++)
					{
						for(k = 0; k < BLOCK_SIZE; k++)         
						{
							for(m = 0; m < BLOCK_SIZE; m++) 
							{
								rgb_pixel_t* pixel1 = pixels1[BLOCK_SIZE*p*width+BLOCK_SIZE*q+m+k*width];
								rgb_pixel_t* pixel2 = pixels2[BLOCK_SIZE*i*width+BLOCK_SIZE*j+m+k*width];
								diff = abs(pixel1->blue - pixel2->blue) + abs(pixel1->green - pixel2->green) + 
								       abs(pixel1->red - pixel2->red) + abs(pixel1->alpha - pixel2->alpha);
								sad = sad + diff; 
							}		  			          
						}					
						sad_MATRIX[p*mb_width+q] = sad;				  
						sad = 0;
					}                 
				}   				
				sad_min = INT_MAX; 
				for(p1 = 0; p1 < mb_height; p1++)
				{
					for(q1 = 0; q1 < mb_width; q1++)
					{
						if(sad_MATRIX[p1*mb_width+q1] == sad_min)                            
						{
							if(abs(q1-i) + abs(p1-j) < abs(row-i) + abs(col-j))
							{
								row = p1;
								col = q1;		
							}
						}
						
						if(sad_MATRIX[p1*mb_width+q1] < sad_min)
						{
							sad_min = sad_MATRIX[p1*mb_width+q1];
							row = p1;
							col = q1;						   
						}
						
					}
				}
				for(k1 = 0; k1 < BLOCK_SIZE; k1++)                  
				{
					for(m1 = 0; m1 < BLOCK_SIZE; m1++) 
					{
						pixels_out[BLOCK_SIZE*i*width+BLOCK_SIZE*j+m1+k1*width] = pixels1[BLOCK_SIZE*row*width+BLOCK_SIZE*col+m1+k1*width];
					}		  			    
				}
				fprintf(pFile, "Block Number %d %d: [%d %d]\n", i, j, col-j, row-i);
			}		 		 		 

		}
		free(sad_MATRIX);
	}
	bmp_out = bmp_create(width, height, 8);
	if (bmp_out != NULL) 
	{
		for (i = 0; i < width; i++) 
		{
			for (j = 0; j < height; j++) 
			{
				bmp_set_pixel(bmp_out, i, j, *(pixels_out[j*width+i]));
			}
		}
		bmp_save(bmp_out, "bmp_out.bmp");
	}
	  
	bmp_destroy(bmp1);
	bmp_destroy(bmp2);
	bmp_destroy(bmp_out);
	
	free(pixels1);
	free(pixels2);
	free(pixels_out);
	fclose(pFile);

	return 0;	

}



#endif 