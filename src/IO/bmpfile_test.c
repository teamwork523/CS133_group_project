// Example of using the modified libbmp APIs.
// The code is particularly written for the term project of
// CS 133 Spring 2012.
//
// Author: Chien-Chia Chen

// To compile: `gcc -lm bmpfile.c bmpfile_test.c`
// To run: `a.out test.bmp 128 256 8`
//
// The code takes 4 arguments:
//   [1] output file name
//   [2] width of the base bmp
//   [3] height of the base bmp
//   [4] depth of the base bmp (MUST be 8 or the code will go crazy)
//
// The code then creates a base BMP using bmp_create and
// fills up some arbitrary pixels.
// The base BMP will be saved and then the code will read it again
// from the file. The BMP read from the file will be saved to
// "bmp_read.bmp".
//
// The last thing it does is to creat another BMP using bmp_create
// with size of (width * 2, height). Then, the right half of this
// new BMP will have the same content as the left half. This gives
// you one way to "enlarge" the BMP size without touching headers
// explicitly.
//
// Please note that regardless of the depth (bpp), libbmp always
// stores raw data in RGB format (see bmpfile_t.pixels). That is,
// although 8-bpp uses only ONE byte for each pixel, internally,
// libbmp still uses FOUR bytes (RGB and Alpha) to represent one
// pixel. The RGB representation will then be converted back to
// color indexies when you call bmp_save to save pixels to a file.
//
// For the purpose of this project, you may safely assume all
// input files are 8-bbp and thus you need only 1 byte for 1 pixel.
// Here's an example of how to obtain an 8-bpp color index for
// a given pixel:
//
// bmpfile_t *bmp; // Pointer points to a BMP image.
// int x = 0, y = 0;
// int color_index = find_closest_color(bmp, *bmp_get_pixel(bmp, x, y));
//
// You may use the above functions to create the data structure
// your image processing library needs, which should be a 2-D
// array storing color indexies. Once you're done with the image
// processing, you may then store the pixels back in rgb_pixel_t
// type, which would be similar to the following:
//
// bmp_set_pixel(bmp, x, y, *get_8bpp_color(bmp, color_index));
//
// After you properly call bmp_set_pixel for each pixel in the
// post-process BMP, you can then call bmp_save to save it
// as a given output file.
//
// If you're not sure what I'm talking about, simply remember this:
// For your image processing functions, use find_closest_color and
// work on the color indexies.
// Once you're done processing and before you save it back to a file,
// call bmp_set_pixel with get_8bpp_color to store it back as an
// RGB pixel.
// And yes, for the above two steps, you have to loop on all pixels
// and do it pixel by pixel. (And yes, they are paralleliz-able.)
//
// You should avoid manipulating on the raw RGB pixels unless you
// are experienced with color representations and know exactly
// what you are doing.

#include <stdio.h>
#include "bmpfile.h"

int main(int argc, char **argv) {
  bmpfile_t *bmp_ori = NULL, *bmp_read = NULL, *bmp_ext = NULL;
  int i, j;
  int width, height, depth;
  rgb_pixel_t pixel = {128, 64, 0, 0};

  if (argc < 5) {
    printf("Usage: %s filename width height depth.\n", argv[0]);
    return 1;
  }

  // Parse argv.
  width = atoi(argv[2]);
  height = atoi(argv[3]);
  depth = atoi(argv[4]);

  // Generate a base BMP file.
  if ((bmp_ori = bmp_create(width, height, depth)) == NULL) {
    printf("Invalid depth value: %s.\n", argv[4]);
    return 1;
  }

  for (i = 10, j = 10; j < height; ++i, ++j) {
    bmp_set_pixel(bmp_ori, i, j, pixel);
    pixel.red++;
    pixel.green++;
    pixel.blue++;
    bmp_set_pixel(bmp_ori, i + 1, j, pixel);
    bmp_set_pixel(bmp_ori, i, j + 1, pixel);
  }

  bmp_save(bmp_ori, argv[1]);

  // Read from the base BMP file.
  printf("reading from file %s ...\n", argv[1]);
  bmp_read = bmp_create_8bpp_from_file(argv[1]);

  if (bmp_read != NULL) {
    printf("header.filesz = %d\n", bmp_get_header(bmp_read).filesz);
    printf("width = %u, height = %u, depth = %hu, bytes = %u\n",
           bmp_get_dib(bmp_read).width,
           bmp_get_dib(bmp_read).height,
           bmp_get_dib(bmp_read).depth,
           bmp_get_dib(bmp_read).bmp_bytesz);
    bmp_save(bmp_read, "bmp_read.bmp");
  }

  // Double the width of the base BMP that has just read.
  bmp_ext = bmp_create(width * 2, height, 8);
  if (bmp_ext != NULL) {
    int i, j;
    for (i = 0; i < width; ++i) {
      for (j = 0; j < height; ++j) {
        rgb_pixel_t *pixel = bmp_get_pixel(bmp_read, i, j);
        bmp_set_pixel(bmp_ext, i, j, *pixel);
        bmp_set_pixel(bmp_ext, i + width, j, *pixel);
      }
    }
    bmp_save(bmp_ext, "bmp_ext.bmp");
  }

  bmp_destroy(bmp_ori);
  bmp_destroy(bmp_read);
  bmp_destroy(bmp_ext);
  return 0;
}
