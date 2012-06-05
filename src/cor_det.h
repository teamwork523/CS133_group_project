#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "IO/bmpfile.h"
#include <limits.h>
#include <math.h>

void cornerDetectionSequential(char* filename);
void cornerDetectionParallel(char* filename, int n_thread);

// input is filename for corners
// output another file corners.bmp
// corners are marked in black
void cornerDetectionSequential(char* filename)
{
	bmpfile_t *bmp_write = NULL;
	bmpfile_t *bmp_read = NULL;
//	printf("Reading file\n");
	bmp_read = bmp_create_8bpp_from_file(filename);
	int height = bmp_get_height(bmp_read);
	int width = bmp_get_width(bmp_read);

	rgb_pixel_t pixel = {0, 50, 0, 0};
	//color of corner
	rgb_pixel_t pixel_corner = {0, 0, 0, 0};
	
	int i = 0, j = 0;
	
	//allocate array storage
	int **image = malloc(height*sizeof(int *));	
	int **intVar = malloc(height*sizeof(int *));	
	int **corner = malloc(height*sizeof(int *));	

	int size = sizeof(int) * width;

	for(i = 0; i<height; i++)
	{
		image[i] = malloc(size);
		intVar[i] = malloc(size);
		corner[i] = malloc(size);
	}
	
	//read the bmp
	for(i = 0; i<width; i++)
	{
		for(j = 0; j < height; j++)
		{
			image[j][i] = find_closest_color(bmp_read,*bmp_get_pixel(bmp_read,i,j));
		}
	}
	//initialze the arrays 
	for(i = 0; i<height; i++)
	{
		//width
		for(j = 0; j < width; j++)
		{
			if(i == 0 || i == 1 || i == height -1 || i == height -2 ||
			   j == 0 || j == 1 || j == height -1 || j == height -2 )
			{
				intVar[i][j] = 0;
				corner[i][j] = 0;
			}
			else
			{
				intVar[i][j] = INT_MAX;
				corner[i][j] = 1;
			}
		}
	}

	int u = 0; int v = 0;
	int h = 0; int w = 0;
	int sum = 0;
	int shift = 0;
	
	//corner detection 3x3 window
//	printf("Computing intensity variation\n");
	for(h = 2; h<height-2; h++)
	{
		for(w = 2; w<width-2; w++)
		{
			//for each shifted window
			for( u = -1; u <= 1; u++)
			{
				for(v = -1; v <= 1; v++)
				{
					if(u != 0 || v != 0)
					{
						sum = 0;
						//calcaute shited window intensity
						for(i = -1; i<=1; i++)
						{
							for(j = -1; j<=1; j++)
							{
								if(h+u+i >= 0 && h+u+i < height)
								{
									if(w+j+v >=0 && w+v < width)
									{ 
										sum += pow(image[h+i][w+j] - image[h+i+u][w+j+v],2);
									}
								}
							}
						}
						//set variation if minimum
						if(sum < intVar[h][w])
						{
							intVar[h][w] = sum;
						}
					}
				}
			}
		}
	}

//	printf("Thresholding interest map\n");
	//Threshold intrest map - set all below threshold T to zero
	int T = 50000;
	for(h = 0; h<height; h++)
	{
		for(w = 0; w<width; w++)
		{
			if(intVar[h][w] < T)
			{
				intVar[h][w] = 0;
			}
		}
	}

//	printf("Performing non-maximal suppression\n");
	//1 mean corner 0 means not corner

	//find local maxima
	//for all pixels
	for(h = 0; h<height; h++)
	{
		for(w = 0; w<width; w++)
		{
			//for all adjacent pixlels
			for(i = -1; i<=1; i++)
			{
				for(j = -1; j<=1; j++)
				{
					if(i != 0 || j != 0)
					{
						//adjacent pixel inside image
						if((i+h >= 0 && i+h < height) && (j+w >=0 && j+w < width))
						{
							//we are not the maximum
							if(intVar[h+i][w+j] > intVar[h][w] || intVar[h][w] == 0)
							{
								corner[h][w] = 0;
							}
						}
					}
				}
			}
		}
	}

//	printf("Outputing File\n");
	//copy image
	bmp_write = bmp_create(width,height,8);
	for(i = 0; i < width; i++)
	{
		for(j = 0; j<height; j++)
		{
			rgb_pixel_t *pixel = bmp_get_pixel(bmp_read, i, j);
			bmp_set_pixel(bmp_write, i, j, *pixel);
		}
	}

	//draw in corners
	for(i = 0; i < width; i++)
	{
		for(j = 0; j<height; j++)
		{
			if(corner[j][i] == 1)
			{
				bmp_set_pixel(bmp_write,i-1,j-1,pixel_corner);
				bmp_set_pixel(bmp_write,i-1,j,pixel_corner);
				bmp_set_pixel(bmp_write,i,j-1,pixel_corner);
				bmp_set_pixel(bmp_write,i+1,j+1,pixel_corner);
				bmp_set_pixel(bmp_write,i+1,j,pixel_corner);
				bmp_set_pixel(bmp_write,i,j+1,pixel_corner);
				bmp_set_pixel(bmp_write,i+1,j-1,pixel_corner);
				bmp_set_pixel(bmp_write,i-1,j+1,pixel_corner);
			}
		}
	}
	bmp_save(bmp_write,"corner.bmp");
	bmp_destroy(bmp_read);
	bmp_destroy(bmp_write);
	
	//free storage
	for(i = 0; i<height; i++)
	{
		free(intVar[i]);
		free(corner[i]);
		free(image[i]);
	}
	free(intVar);
	free(corner);
	free(image);

	return;
}

// input is filename for corners
// output another file corners.bmp
// corners are marked in black
void cornerDetectionParallel(char* filename, int n_thread)
{
//	printf("%i threads\n",n_thread);
	//set number of threads
	omp_set_num_threads(n_thread);

	bmpfile_t *bmp_write = NULL;
	bmpfile_t *bmp_read = NULL;
//	printf("Reading file\n");
	bmp_read = bmp_create_8bpp_from_file(filename);

	int height = bmp_get_height(bmp_read);
	int width = bmp_get_width(bmp_read);
	rgb_pixel_t pixel = {0, 50, 0, 0};
	//color of corner
	rgb_pixel_t pixel_corner = {0, 0, 0, 0};
	
	int i = 0, j = 0;
	//allocate array storage
	int **image = malloc(height*sizeof(int *));	
	int **intVar = malloc(height*sizeof(int *));	
	int **corner = malloc(height*sizeof(int *));	

	int size = sizeof(int) * width;
	for(i = 0; i<height; i++)
	{
		image[i] = malloc(size);
		intVar[i] = malloc(size);
		corner[i] = malloc(size);
	}

	//read the image
	#pragma omp parallel for shared(image,bmp_read) private(i,j)
	for(i = 0; i<width; i++)
	{
		for(j = 0; j < height; j++)
		{
			image[j][i] = find_closest_color(bmp_read,*bmp_get_pixel(bmp_read,i,j));
		}
	} 
	
//	printf("Calculating intensity variation\n");
	//initialze array
	#pragma omp parallel for shared(intVar, corner) private(i,j)
	for(i = 0; i<height; i++)
	{
		//width
		for(j = 0; j < width; j++)
		{
			if(i == 0 || i == 1 || i == height -1 || i == height -2 ||
			   j == 0 || j == 1 || j == height -1 || j == height -2 )
			{
				intVar[i][j] = 0;
				corner[i][j] = 0;
			}
			else
			{
				intVar[i][j] = INT_MAX;
				corner[i][j] = 1;
			}
		}
	}

	int u = 0; int v = 0;
	int h = 0; int w = 0;
	int sum = 0;
	int shift = 0;

	//compute interst values using 3x3 square
	#pragma omp parallel for shared(intVar,image) private(i,j, u, v, h, w,sum)
	for(h = 2; h<height-2; h++)
	{
		for(w = 2; w<width-2; w++)
		{
			//for each shifted window
			for( u = -1; u <= 1; u++)
			{
				for(v = -1; v <= 1; v++)
				{
					if(u != 0 || v != 0)
					{
						sum = 0;
						//calcaute shited window intensity
						for(i = -1; i<=1; i++)
						{
							for(j = -1; j<=1; j++)
							{
								if(h+u+i >= 0 && h+u+i < height)
								{
									if(w+j+v >=0 && w+v < width)
									{ 
										sum += pow(image[h+i][w+j] - image[h+i+u][w+j+v],2);
									}
								}
							}
						}
						//set variation if minimum
						if(sum < intVar[h][w])
						{
							intVar[h][w] = sum;
						}
					}
				}
			}
		}
	}

	//Threshold intrest map - set all below threshold T to zero
	int T = 50000;
//	printf("Thresholding interest map\n");
	#pragma omp parallel for shared(intVar) private(h,w)
	for(h = 0; h<height; h++)
	{
		for(w = 0; w<width; w++)
		{
			if(intVar[h][w] < T)
			{
				intVar[h][w] = 0;
			}
		}
	}

//	printf("Performing non-maximal supression\n");
	//1 mean corner 0 means not corner
	//find local maxima
	//for all pixels
	#pragma omp parallel for shared(corner,intVar) private(h,w,i,j)
	for(h = 0; h<height; h++)
	{
		for(w = 0; w<width; w++)
		{
			//for all adjacent pixlels
			for(i = -1; i<=1; i++)
			{
				for(j = -1; j<=1; j++)
				{
					if(i != 0 || j != 0)
					{
						if((i+h >= 0 && h+i < height) && (j+w >=0 && j+w < width))
						{
							if(intVar[h+i][w+j] > intVar[h][w] || intVar[h][w] == 0)
							{
								corner[h][w] = 0;
							}
						}
					}
				}
			}
		}
	}
//	printf("Outputing file\n");
	//copy image
	bmp_write = bmp_create(width,height,8);
	#pragma omp parallel for shared(bmp_write, bmp_read) private(i,j)
	for(i = 0; i < width; i++)
	{
		for(j = 0; j<height; j++)
		{
			rgb_pixel_t *pixel = bmp_get_pixel(bmp_read, i, j);
			bmp_set_pixel(bmp_write, i, j, *pixel);
		}
	}

	//draw in corners
	#pragma omp parallel for shared(bmp_write,pixel_corner) private(i,j)
	for(i = 0; i < width; i++)
	{
		for(j = 0; j<height; j++)
		{
			if(corner[j][i] == 1)
			{
				bmp_set_pixel(bmp_write,i-1,j-1,pixel_corner);
				bmp_set_pixel(bmp_write,i-1,j,pixel_corner);
				bmp_set_pixel(bmp_write,i,j-1,pixel_corner);
				bmp_set_pixel(bmp_write,i+1,j+1,pixel_corner);
				bmp_set_pixel(bmp_write,i+1,j,pixel_corner);
				bmp_set_pixel(bmp_write,i,j+1,pixel_corner);
				bmp_set_pixel(bmp_write,i+1,j-1,pixel_corner);
				bmp_set_pixel(bmp_write,i-1,j+1,pixel_corner);
			}
		}
	}
	bmp_save(bmp_write,"corner.bmp");
	bmp_destroy(bmp_read);
	bmp_destroy(bmp_write);
	
	//free storage
	for(i = 0; i<height; i++)
	{
		free(intVar[i]);
		free(corner[i]);
		free(image[i]);
	}
	free(intVar);
	free(corner);
	free(image);
	return;
}

