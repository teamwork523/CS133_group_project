/* CS 133 Parallel & Distributed Computing Final Project 
 * -- Image Operation APIs Optimized through OpenCV
 * 
 * To Compile: gcc -fopenmp -o imgProj main.c IO/bmpfile.c -lm
 * To Run all operations: ./imgProj img_path1 img_path2 deg scale_fac sigma
 * 
 * If you want to compile for each operation seperately,
 * you can run the program with secific options as following usages.
 *
 * Usage:
 * 1. Motion Estimation: ./imgProj -m img_path1 img_path2
 *                       ./imgProj -mp img_path1 img_path2 (num_threads chunk_size)
 * 2. Corner Detection:  ./imgProj -c img_path1
 *                       ./imgProj -cp img_path1 (num_threads chunk_size)
 * 3. Rotation:          ./imgProj -r img_path1 deg
 *                       ./imgProj -rp img_path1 deg (num_threads chunk_size)
 * 4. Scaling:           ./imgProj -s img_path1 scale_fac
 *                       ./imgProj -sp img_path1 scale_fac (num_threads chunk_size)
 * 5. Gaussian Blur:     ./imgProj -g img_path1 sigma
 *                       ./imgProj -gp img_path1 sigma (num_threads chunk_size)
 * 6. Outline Detection: ./imgProj -o img_path1
 *                       ./imgProj -op img_path1 (num_threads chunk_size)
 * 
 * Author: Haokun Luo
 *         Xin Zhang
 *         Ding Zhao
 *         Mengyi Zhu
 *         Michael Koyama
 *         Ali Mousavi
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "IO/bmpfile.h"
#include "mot_est.h"
#include "cor_det.h"
#include "rotation.h"
#include "scaling.h"
#include "g_blur.h"
#include "outline_det.h"

void printUsage() {
    printf("********************************************************************\n");
    printf("CS 133 Parallel & Distributed Computing Final Project\n");
    printf("-- Image Operation APIs Optimized through OpenCV\n\n");
    printf("Usage:\n");
    printf("> NOTICE: parameters in parenthesis is optional\n");
    printf("> Run all the operations in sequential:\n>\
            ./imgProj img_path1 img_path2 deg scale_fac sigma\n");
    printf("> Run all the operations in parallel:\n>\
            ./imgProj -p img_path1 img_path2 deg scale_fac sigma (num_threads chunk_size)\n");
    printf("> Run Motion Estimation in sequential:\n>\
            ./imgProj -m img_path1 img_path2\n");
    printf("> Run Motion Estimation in parallel:\n>\
            ./imgProj -mp img_path1 img_path2 (num_threads chunk_size)\n");
    printf("> Run Corner Detection in sequential:\n>\
            ./imgProj -c img_path1\n");
    printf("> Run Corner Detection in parallel:\n>\
            ./imgProj -cp img_path1 (num_threads chunk_size)\n");
    printf("> Run Rotation in sequential:\n>\
            ./imgProj -r img_path1 deg\n");
    printf("> Run Rotation in parallel:\n>\
            ./imgProj -rp img_path1 deg (num_threads chunk_size)\n");
    printf("> Run Scaling in sequential:\n>\
            ./imgProj -s img_path1 scale_fac\n");
    printf("> Run Scaling in parallel:\n>\
            ./imgProj -sp img_path1 scale_fac (num_threads chunk_size)\n");
    printf("> Run Gaussian Blur in sequential:\n>\
            ./imgProj -g img_path1 sigma\n");
    printf("> Run Gaussian Blur in parallel:\n>\
            ./imgProj -gp img_path1 sigma (num_threads chunk_size)\n");
    printf("> Run Outline Detection in sequential:\n>\
            ./imgProj -o img_path1\n");
    printf("> Run Outline Detection in parallel:\n>\
            ./imgProj -op img_path1 (num_threads chunk_size)\n");
    printf("********************************************************************\n");
}

int main(int argc, char **argv) {
    char *input_img1 = NULL, *input_img2 = NULL;
    double deg = 0.0, scale_fac = 0.0, sigma = 0.0;
    int num_threads = 1, chunk_size = 1;
    
    // check input parameters
    if (argc <= 6){
        if (argc < 2){
            printUsage();
            exit(1);
        }            
        // parse the input parameters
        if (!strcmp(argv[1], "-m") || !strcmp(argv[1], "-mp")){
            printf("Motion Estimation in process ...\n");
            if (argc < 4 || argc > 6){
                printUsage();
                exit(1);
            }
            // assign parameters
            input_img1 = argv[2];
            input_img2 = argv[3];
            if(strcmp(argv[1], "-m") == 0)
			{
				motion_estimation(input_img1, input_img2);
			}
			else
			{
				motion_estimation_parallel(input_img1, input_img2);
			}
            
        }
        else if (!strcmp(argv[1], "-c") || !strcmp(argv[1], "-cp")){
            printf("Corner Detection in process ...\n");
            if (argc < 3 || argc > 5){
                printUsage();
                exit(1);
            }
            // assign parameters
            input_img1 = argv[2];
            
			 if(strcmp(argv[1], "-c") == 0)
			{
				cornerDetectionSequential(input_img1);
			}
			else
			{
				num_threads = 8;
                if (argc == 4){
                    num_threads = atoi(argv[3]);
                }
				cornerDetectionParallel(input_img1, num_threads);
			}

            
        }
        else if (!strcmp(argv[1], "-r") || !strcmp(argv[1], "-rp")){
            printf("Rotation in process ...\n");
            if (argc < 4 || argc > 6){
                printUsage();
                exit(1);
            }
            // assign parameters
            input_img1 = argv[2];
            deg = atof(argv[3]);
            
            if (!strcmp(argv[1], "-r")){
                rotation(input_img1, deg);
            }
            else {
                // assigne the number of threads and chunk size
                if (argc == 5){
                    num_threads = atoi(argv[4]);
                }
                else {
                    num_threads = atoi(argv[4]);
                    chunk_size = atoi(argv[5]);
                }
                rotation_parallel(input_img1, deg, num_threads, chunk_size);
            }
        }
        else if (!strcmp(argv[1], "-s") || !strcmp(argv[1], "-sp")){
            printf("Scaling in process ...\n");
            if (argc < 4 || argc > 6){
                printUsage();
                exit(1);
            }
            
            // --------------------Done by Mengyi Zhu----------------------------------
            //---------------------Here is scaling operation---------------------------
            //--------------------it will store the scaled version into scaling.bmp----
            // assign parameters
            input_img1 = argv[2];
            scale_fac = atof(argv[3]);
            num_threads = 1;
            
            //---------------------implemention the operation--------------------------
            if (!strcmp(argv[1], "-s"))
            	image_scaling(scale_fac, input_img1,"scaling.bmp");
            else if(!strcmp(argv[1], "-sp"))
            {
            	if (argc == 5)
            		num_threads = atoi(argv[4]);	
            	image_scaling_parallel(scale_fac, input_img1,"scaling.bmp", num_threads);
            }
            //-------------------------------------------------------------------------
            //-------------------------------------------------------------------------
            
        }
        else if (!strcmp(argv[1], "-g") || !strcmp(argv[1], "-gp")){
            printf("Gasussian Blur in process ...\n");
            if (argc < 4 || argc > 6){
                printUsage();
                exit(1);
            }
            // assign parameters
            input_img1 = argv[2];
            sigma = atof(argv[3]);
            
            if (!strcmp(argv[1], "-g")){
                gaussian_blur(input_img1, sigma);
            }
            else {
                // assigne the number of threads and chunk size
                if (argc == 5){
                    num_threads = atoi(argv[4]);
                }
                else {
                    num_threads = atoi(argv[4]);
                    chunk_size = atoi(argv[5]);
                }
                gaussian_blur_parallel(input_img1, sigma, num_threads, chunk_size);
            }
            
        }
        else if (!strcmp(argv[1], "-o") || !strcmp(argv[1], "-op")){
            printf("Outline Detection in process ...\n");
            if (argc < 3 || argc > 5){
                printUsage();
                exit(1);
            }
            // assign parameters
            input_img1 = argv[2];
            
            // TODO: Ali, add your code here
            
        }
        else {
            if (argc == 6){
                printf("All-in-one in process ...\n");
                
                // assign parameters
                input_img1 = argv[1];
                input_img2 = argv[2];
                deg = atof(argv[3]);
                scale_fac = atof(argv[4]);
                sigma = atof(argv[5]);
                
                // TODO: Ding, add your sequential code here
                // Michael's sequential code here
				cornerDetectionSequential(input_img1);
                // TODO: Haokun, add your sequential code here
                // --------------------Done by Mengyi Zhu---------------------------------
                //---------------------Here is scaling operation--------------------------
                //--------------------it will store the scaled version into scaling.bmp---
                image_scaling(scale_fac, input_img1,"scaling.bmp");
                //------------------------------------------------------------------------
                //------------------------------------------------------------------------
                // Xin, add your sequential code here
                gaussian_blur(input_img1, sigma);
                // TODO: Ali, add your sequential code here
            }
            else if (argc >= 7 && argc <= 9 && !strcmp(argv[1], "-p")){
                // assign parameters
                input_img1 = argv[2];
                input_img2 = argv[3];
                deg = atof(argv[4]);
                scale_fac = atof(argv[5]);
                sigma = atof(argv[6]);
                
                if (argc == 8){
                    num_threads = atoi(argv[7]);
                }
                else if (argc == 9){
                    num_threads = atoi(argv[7]);
                    chunk_size = atoi(argv[8]);
                }
                
                // TODO: Ding, add your parallel code here
                // TODO: Michael, add your parallel code here
				cornerDetectionParallel(input_img1,num_threads);
                // TODO: Haokun, add your parallel code here
                // --------------------Done by Mengyi Zhu---------------------------------
                //---------------------Here is scaling operation-------------------------
                //--------------------it will store the scaled version into scaling.bmp----
                image_scaling_parallel(scale_fac, input_img1,"scaling.bmp", num_threads);
                //----------------------------------------------------------------------
                //----------------------------------------------------------------------
                // Xin, add your parallel code here
                gaussian_blur_parallel(input_img1, sigma, num_threads, chunk_size);
                // TODO: Ali, add your parallel code here
            }
            else {
                printf("Wrong option passed in or not enough parameters. Please refer the following usage.\n");
                printUsage();
                exit(1);
            }
        }
    }
    else {
        printUsage();
        exit(1);
    }
    
    return 0;   
}
