#include<omp.h>
#include<stdio.h>
#include<stdlib.h>
#include"IO/bmpfile.h"



void resizeBilinear(rgb_pixel_t* pixels, int w, int h, int w2, int h2,rgb_pixel_t* new_pixels, int thread ) 
{
    rgb_pixel_t a, b, c, d;
    int x, y, index ;
    float x_ratio = ((float)(w-1))/w2 ;
    float y_ratio = ((float)(h-1))/h2 ;
    float x_diff, y_diff, blue, red, green ;
    int offset = 0 ;
    int i, j;
    
    int thread_num = thread;
    #pragma omp parallel for num_threads(thread_num) firstprivate(x, y, index, x_diff, y_diff,a,b,c,d,j,offset) shared(new_pixels,pixels,i)
    for (i=0;i<h2;i++) {
        for (j=0;j<w2;j++) {   
            x = (int)(x_ratio * j) ;
            y = (int)(y_ratio * i) ;
            x_diff = (x_ratio * j) - x ;
            y_diff = (y_ratio * i) - y ;
            index = (y*w+x) ;             
            a = pixels[index] ;
            b = pixels[index+1] ;
            c = pixels[index+w] ;
            d = pixels[index+w+1] ;
	     	
            // blue element
            // Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
            new_pixels[offset].blue = (a.blue)*(1-x_diff)*(1-y_diff) + (b.blue)*(x_diff)*(1-y_diff) +
                   (c.blue)*(y_diff)*(1-x_diff)   + (d.blue)*(x_diff*y_diff);

            // green element
            // Yg = Ag(1-w)(1-h) + Bg(w)(1-h) + Cg(h)(1-w) + Dg(wh)
            new_pixels[offset].green = (a.green)*(1-x_diff)*(1-y_diff) + (b.green)*(x_diff)*(1-y_diff) +
                    (c.green)*(y_diff)*(1-x_diff)   + (d.green)*(x_diff*y_diff);

            // red element
            // Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
            new_pixels[offset].red = (a.red)*(1-x_diff)*(1-y_diff) + (b.red)*(x_diff)*(1-y_diff) +
                  (c.red)*(y_diff)*(1-x_diff)   + (d.red)*(x_diff*y_diff);
		 
		        offset= i*w2+j;
        }
    }
}

//non_parrallel version
int image_scaling(double ratio,char* old_name, char* name){
  bmpfile_t* bmp_read = bmp_create_8bpp_from_file(old_name);
  if (bmp_read == NULL)
  {
	printf("The input file does not exist\n");
	return 1;
  }
  int width = bmp_get_dib(bmp_read).width;
  int height = bmp_get_dib(bmp_read).height;
  int new_width = (int) (width * ratio);
  int new_height = (int) (height * ratio);
  bmpfile_t* bmp_rsz = bmp_create(new_width, new_height, 8);
 
  if (bmp_rsz != NULL) { 	
		rgb_pixel_t * pixels = malloc(sizeof(rgb_pixel_t) * width * height);
		rgb_pixel_t * new_pixels = malloc(sizeof(rgb_pixel_t) * new_width * new_height);
		int i, j;
		for (i = 0 ; i < height; ++i) 
			for (j = 0; j < width; ++j) 
				pixels[i*width+ j] = *bmp_get_pixel(bmp_read, j,i );
      
		
		resizeBilinear(pixels, width, height,new_width,new_height, new_pixels,1);
		
		for (j = 0; j < new_height; ++j)
			for (i = 0; i < new_width; ++i)
				bmp_set_pixel(bmp_rsz, i, j, new_pixels[j*new_width+i]);
		bmp_save(bmp_rsz, name);
		return 0;
  }
  return 1;
}



//non_parrallel version
int image_scaling_parallel(double ratio,char* old_name, char* name, int num_thread){
  bmpfile_t* bmp_read = bmp_create_8bpp_from_file(old_name);
  if (bmp_read == NULL)
  {
  printf("The input file does not exist\n");
	return 1;
  }
  int width = bmp_get_dib(bmp_read).width;
  int height = bmp_get_dib(bmp_read).height;
  int new_width = (int) (width * ratio);
  int new_height = (int) (height * ratio);
  bmpfile_t* bmp_rsz = bmp_create(new_width, new_height, 8);
 
  if (bmp_rsz != NULL) { 	
		rgb_pixel_t * pixels = malloc(sizeof(rgb_pixel_t) * width * height);
		rgb_pixel_t * new_pixels = malloc(sizeof(rgb_pixel_t) * new_width * new_height);
		int i, j;
		for (i = 0 ; i < height; ++i) 
			for (j = 0; j < width; ++j) 
				pixels[i*width+ j] = *bmp_get_pixel(bmp_read, j,i );
      
		
		resizeBilinear(pixels, width, height,new_width,new_height, new_pixels,num_thread);
		
		for (j = 0; j < new_height; ++j)
			for (i = 0; i < new_width; ++i)
				bmp_set_pixel(bmp_rsz, i, j, new_pixels[j*new_width+i]);
		bmp_save(bmp_rsz, name);
		return 0;
  }
  return 1;
}
