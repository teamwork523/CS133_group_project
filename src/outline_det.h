#include <stdio.h>
#include <omp.h>



bmpfile_t* gray_scale(bmpfile_t *bmp_read, int w, int h){
	int i,j,gray;
	rgb_pixel_t pixel;
	for (i = 0; i <w; i++) {
	 for (j=0; j<h; j++){
		 pixel = *bmp_get_pixel(bmp_read, i, j);
		 gray=((pixel.red*0.3)+(pixel.green*0.59)+(pixel.blue*0.11))/3;
		 pixel.red=gray;
		 pixel.green=gray;
		 pixel.blue=gray;
	     	 bmp_set_pixel(bmp_read, i, j, pixel);
	 }
	}
	return bmp_read;
}

int get_pixel(bmpfile_t *bmp_read, int x, int y){
	rgb_pixel_t pixel=*bmp_get_pixel(bmp_read, x, y);
	return pixel.blue;
}


bmpfile_t* result(bmpfile_t *bmp_read, int w, int h){
	bmpfile_t *bmp = bmp_create(w, h, 8);
	rgb_pixel_t pixel;
	int i,j,p0,p1,p2,p3,p4,p5,p6,p7,p8;
	float filter=1.9;
	for (i = 0; i <w; i++) {
	 for (j=1; j<h; j++){
		 p0=get_pixel(bmp_read, i, j);
		 if (i==0){
			 if (j==0){}
			 else{
				 p1=get_pixel(bmp_read, i, j-1);
				 if ((p0>p1*filter)||(p1>p0*filter)){
					pixel.blue=255;
					pixel.green=255;
					pixel.red=255;
				 }
				else{
					pixel.blue=0;
					pixel.green=0;
					pixel.red=0;
				}
			 }
		 }
		 else if (j==0){
			p1=get_pixel(bmp_read, i-1, j);
			p2=get_pixel(bmp_read, i-1, j+1);
		        if ((p0>p1*filter)||(p1>p0*filter)||(p0>p2*filter)||(p2>p0*filter)){
				pixel.blue=255;
				pixel.green=255;
				pixel.red=255;
			 }
			else{
				pixel.blue=0;
				pixel.green=0;
				pixel.red=0;
			}
		 }
		 else if(j==(h-1)){
			  p1=get_pixel(bmp_read, i, j-1);
			 p2=get_pixel(bmp_read, i-1, j-1);
			 p3=get_pixel(bmp_read, i-1, j);
			 if ((p0>p1*filter)||(p1>p0*filter)||(p0>p2*filter)||(p2>p0*filter)||(p0>p3*filter)||(p3>p0*filter)){
				pixel.blue=255;
				pixel.green=255;
				pixel.red=255;
			 }
			else{
				pixel.blue=0;
				pixel.green=0;
				pixel.red=0;
			}
		 }

		 else{
			 p1=get_pixel(bmp_read, i, j-1);
			 p2=get_pixel(bmp_read, i-1, j-1);
			 p3=get_pixel(bmp_read, i-1, j);
			 p4=get_pixel(bmp_read, i-1, j+1);
			 if ((p0>p1*filter)||(p1>p0*filter)||(p0>p2*filter)||(p2>p0*filter)||(p0>p3*filter)||(p3>p0*filter)||(p0>p4*filter)||(p4>p0*filter)){
				pixel.blue=255;
				pixel.green=255;
				pixel.red=255;
			 }
			else{
				pixel.blue=0;
				pixel.green=0;
				pixel.red=0;
			}
		 }
		    bmp_set_pixel(bmp, i, j, pixel);
	 }
	}
	return bmp;
}

int detection(char *image)
{
  bmpfile_t *bmp, *bmp_read;

  bmp_read = bmp_create_8bpp_from_file(image);

  int w=bmp_get_width(bmp_read);
  int h=bmp_get_height(bmp_read);


  bmp_read=gray_scale(bmp_read,w,h);
  bmp=result(bmp_read,w,h);

  bmp_save(bmp, "obj_detcted.bmp");
  bmp_destroy(bmp_read);
  bmp_destroy(bmp);

  return 0;
}



//
//
//
//
//
//
//
//
//
//
//
//
//PARALLEL SECTIOn


bmpfile_t* gray_scale_p(bmpfile_t *bmp_read, int w, int h, int num_thr, int chunk_size){
	int i,j,gray;
	rgb_pixel_t pixel;
	omp_set_num_threads(num_thr);
	#pragma omp parallel for private(i,j, gray, pixel) schedule(dynamic,chunk_size)
	for (i = 0; i <w; i++) {
	 for (j=0; j<h; j++){
		 pixel = *bmp_get_pixel(bmp_read, i, j);
		 gray=((pixel.red*0.3)+(pixel.green*0.59)+(pixel.blue*0.11))/3;
		 pixel.red=gray;
		 pixel.green=gray;
		 pixel.blue=gray;
	     	 bmp_set_pixel(bmp_read, i, j, pixel);
	 }
	}
	return bmp_read;
}


bmpfile_t* result_p(bmpfile_t *bmp_read, int w, int h, int num_thr, int chunk_size){
	bmpfile_t *bmp = bmp_create(w,h,8);
	rgb_pixel_t pixel;
	int i,j,p0,p1,p2,p3,p4,p5,p6,p7,p8;
	float filter=1.9;
	omp_set_num_threads(num_thr);
	#pragma omp parallel for private(i,j, p0,p1,p2,p3,p4,p5,p6,p7,p8,pixel) schedule(dynamic,chunk_size)
	for (i = 0; i <w; i++) {
	 for (j=1; j<h; j++){
		 p0=get_pixel(bmp_read, i, j);
		 if (i==0){
			 if (j==0){}
			 else{
				 p1=get_pixel(bmp_read, i, j-1);
				 if ((p0>p1*filter)||(p1>p0*filter)){
					pixel.blue=255;
					pixel.green=255;
					pixel.red=255;
				 }
				else{
					pixel.blue=0;
					pixel.green=0;
					pixel.red=0;
				}
			 }
		 }
		 else if (j==0){
			p1=get_pixel(bmp_read, i-1, j);
			p2=get_pixel(bmp_read, i-1, j+1);
		        if ((p0>p1*filter)||(p1>p0*filter)||(p0>p2*filter)||(p2>p0*filter)){
				pixel.blue=255;
				pixel.green=255;
				pixel.red=255;
			 }
			else{
				pixel.blue=0;
				pixel.green=0;
				pixel.red=0;
			}
		 }
		 else if(j==(h-1)){
			  p1=get_pixel(bmp_read, i, j-1);
			 p2=get_pixel(bmp_read, i-1, j-1);
			 p3=get_pixel(bmp_read, i-1, j);
			 if ((p0>p1*filter)||(p1>p0*filter)||(p0>p2*filter)||(p2>p0*filter)||(p0>p3*filter)||(p3>p0*filter)){
				pixel.blue=255;
				pixel.green=255;
				pixel.red=255;
			 }
			else{
				pixel.blue=0;
				pixel.green=0;
				pixel.red=0;
			}
		 }

		 else{
			 p1=get_pixel(bmp_read, i, j-1);
			 p2=get_pixel(bmp_read, i-1, j-1);
			 p3=get_pixel(bmp_read, i-1, j);
			 p4=get_pixel(bmp_read, i-1, j+1);
			 if ((p0>p1*filter)||(p1>p0*filter)||(p0>p2*filter)||(p2>p0*filter)||(p0>p3*filter)||(p3>p0*filter)||(p0>p4*filter)||(p4>p0*filter)){
				pixel.blue=255;
				pixel.green=255;
				pixel.red=255;
			 }
			else{
				pixel.blue=0;
				pixel.green=0;
				pixel.red=0;
			}
		 }
		    bmp_set_pixel(bmp, i, j, pixel);
	 }
	}
	return bmp;
}



int detection_parallel(char *image, int num_thread, int chunk_size)
{
  bmpfile_t *bmp, *bmp_read;

  bmp_read = bmp_create_8bpp_from_file(image);

  int w=bmp_get_width(bmp_read);
  int h=bmp_get_height(bmp_read);


  bmp_read=gray_scale_p(bmp_read,w,h,num_thread, chunk_size);
  bmp=result_p(bmp_read,w,h,num_thread, chunk_size);

  bmp_save(bmp, "obj_detcted_p.bmp");
  bmp_destroy(bmp_read);
  bmp_destroy(bmp);

  return 0;
}
