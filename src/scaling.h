#ifndef SCALING_H
#define SCALING_H
#include<omp.h>
#include<stdio.h>
#include<stdlib.h>
#include"IO/bmpfile.h"


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
	rgb_pixel_t a, b, c, d,temp;
	int x, y, index ;
	float x_ratio = ((float)(width-1))/new_width ;
	float y_ratio = ((float)(height-1))/new_height ;
	float x_diff, y_diff;
	int i, j;
	for (i=0;i<new_height;i++){
        for (j=0;j<new_width;j++) {   
            x = (int)(x_ratio * j) ;
            y = (int)(y_ratio * i) ;
            x_diff = (x_ratio * j) - x ;
            y_diff = (y_ratio * i) - y ;
            index = (y*width+x) ;             
            a = *bmp_get_pixel(bmp_read, x,y);
            b = *bmp_get_pixel(bmp_read, x+1,y);
            c = *bmp_get_pixel(bmp_read, x,y+1);
            d = *bmp_get_pixel(bmp_read, x+1,y+1);
	     	
            // blue element
            // Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
            temp.blue = (a.blue)*(1-x_diff)*(1-y_diff) + (b.blue)*(x_diff)*(1-y_diff) +
                   (c.blue)*(y_diff)*(1-x_diff)   + (d.blue)*(x_diff*y_diff);

            // green element
            // Yg = Ag(1-w)(1-h) + Bg(w)(1-h) + Cg(h)(1-w) + Dg(wh)
            temp.green = (a.green)*(1-x_diff)*(1-y_diff) + (b.green)*(x_diff)*(1-y_diff) +
                    (c.green)*(y_diff)*(1-x_diff)   + (d.green)*(x_diff*y_diff);

            // red element
            // Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
            temp.red = (a.red)*(1-x_diff)*(1-y_diff) + (b.red)*(x_diff)*(1-y_diff) +
                  (c.red)*(y_diff)*(1-x_diff)   + (d.red)*(x_diff*y_diff);
		 
	    bmp_set_pixel(bmp_rsz, j, i, temp);       
        }
	}
	bmp_save(bmp_rsz, name);
	bmp_destroy(bmp_read);
    	bmp_destroy(bmp_rsz);
	return 0;
  }
  return 1;
}



//parrallel version
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
	float x_ratio = ((float)(width-1))/new_width ;
	float y_ratio = ((float)(height-1))/new_height ;
	
	int i, j;
	omp_lock_t lock;
	omp_init_lock (&lock);
	#pragma omp parallel for num_threads(num_thread) firstprivate(j) schedule(dynamic,1)
	for (i=0;i<new_height;i++){
        for (j=0;j<new_width;j++) {   
	    
	    rgb_pixel_t temp;
            int x = (int)(x_ratio * j) ;
            int y = (int)(y_ratio * i) ;
            float x_diff = (x_ratio * j) - x ;
            float y_diff = (y_ratio * i) - y ;             
            rgb_pixel_t a = *bmp_get_pixel(bmp_read, x,y);
            rgb_pixel_t b = *bmp_get_pixel(bmp_read, x+1,y);
            rgb_pixel_t c = *bmp_get_pixel(bmp_read, x,y+1);
            rgb_pixel_t d = *bmp_get_pixel(bmp_read, x+1,y+1);
	     	
            // blue element
            // Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
            temp.blue = (a.blue)*(1-x_diff)*(1-y_diff) + (b.blue)*(x_diff)*(1-y_diff) +
                   (c.blue)*(y_diff)*(1-x_diff)   + (d.blue)*(x_diff*y_diff);

            // green element
            // Yg = Ag(1-w)(1-h) + Bg(w)(1-h) + Cg(h)(1-w) + Dg(wh)
            temp.green = (a.green)*(1-x_diff)*(1-y_diff) + (b.green)*(x_diff)*(1-y_diff) +
                    (c.green)*(y_diff)*(1-x_diff)   + (d.green)*(x_diff*y_diff);

            // red element
            // Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
            temp.red = (a.red)*(1-x_diff)*(1-y_diff) + (b.red)*(x_diff)*(1-y_diff) +
                  (c.red)*(y_diff)*(1-x_diff)   + (d.red)*(x_diff*y_diff);
	    //omp_set_lock(&lock);
	    bmp_set_pixel(bmp_rsz, j, i, temp);
	    //omp_unset_lock(&lock);	    
        }
	}
	bmp_save(bmp_rsz, name);
	bmp_destroy(bmp_read);
    	bmp_destroy(bmp_rsz);
	return 0;
  }
  return 1;
}
#endif
