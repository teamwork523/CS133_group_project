/* CS 133 Parallel & Distributed Computing Final Project 
 * -- Image Operation APIs Optimized through OpenCV
 * 
 * To Compile: gcc -o imgProj main.c IO/bmpfile.c -lm
 * To Run all operations: ./imgProj img_path1 img_path2 deg scale_fac sigma
 * 
 * If you want to compile for each operation seperately,
 * you can run the program with secific options as following usages.
 *
 * Usage:
 * 1. Motion Estimation: ./imgProj -m img_path1 img_path2
 * 2. Corner Detection:  ./imgProj -c img_path1
 * 3. Rotation:          ./imgProj -r img_path1 deg
 * 4. Scaling:           ./imgProj -s img_path1 scale_fac
 * 5. Gaussian Blur:     ./imgProj -g img_path1 sigma
 * 6. Outline Detection: ./imgProj -o img_path1
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
    printf("> To run all the operations in sequential:     ./imgProj img_path1 img_path2 deg scale_fac sigma\n");
    printf("> To run all the operations in parallel:       ./imgProj -p img_path1 img_path2 deg scale_fac sigma\n");
    printf("> To run Motion Estimation only in sequential: ./imgProj -m img_path1 img_path2\n");
    printf("> To run Motion Estimation only in parallel:   ./imgProj -mp img_path1 img_path2\n");
    printf("> To run Corner Detection only in sequential:  ./imgProj -c img_path1\n");
    printf("> To run Corner Detection only in parallel:    ./imgProj -cp img_path1\n");
    printf("> To run Rotation only in sequential:          ./imgProj -r img_path1 deg\n");
    printf("> To run Rotation only in parallel:            ./imgProj -rp img_path1 deg\n");
    printf("> To run Scaling only in sequential:           ./imgProj -s img_path1 scale_fac\n");
    printf("> To run Scaling only in parallel:             ./imgProj -sp img_path1 scale_fac\n");
    printf("> To run Gaussian Blur only in sequential:     ./imgProj -g img_path1 sigma\n");
    printf("> To run Gaussian Blur only in parallel:       ./imgProj -gp img_path1 sigma\n");
    printf("> To run Outline Detection only in sequential: ./imgProj -o img_path1\n");
    printf("> To run Outline Detection only in parallel:   ./imgProj -op img_path1\n");
    printf("********************************************************************\n");
}

int main(int argc, char **argv) {
    char *input_img1 = NULL, *input_img2 = NULL;
    double deg = 0.0, scale_fac = 0.0, sigma = 0.0;
    
    // check input parameters
    if (argc <= 6){
        if (argc < 2){
            printUsage();
            exit(1);
        }            
        // parse the input parameters
        if (!strcmp(argv[1], "-m") || !strcmp(argv[1], "-mp")){
            printf("Motion Estimation in process ...\n");
            if (argc != 4){
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
            if (argc != 3){
                printUsage();
                exit(1);
            }
            // assign parameters
            input_img1 = argv[2];
            
            // TODO: Michael, add your code here
            
        }
        else if (!strcmp(argv[1], "-r") || !strcmp(argv[1], "-rp")){
            printf("Rotation in process ...\n");
            if (argc != 4){
                printUsage();
                exit(1);
            }
            // assign parameters
            input_img1 = argv[2];
            deg = atof(argv[3]);
            
            // TODO: Haokun, add your code here
            
        }
        else if (!strcmp(argv[1], "-s") || !strcmp(argv[1], "-sp")){
            printf("Scaling in process ...\n");
            if (argc != 4){
                printUsage();
                exit(1);
            }
            // assign parameters
            input_img1 = argv[2];
            scale_fac = atof(argv[3]);
            
            // TODO: Mengyi, add your code here
            
        }
        else if (!strcmp(argv[1], "-g") || !strcmp(argv[1], "-gp")){
            printf("Gasussian Blur in process ...\n");
            if (argc != 4){
                printUsage();
                exit(1);
            }
            // assign parameters
            input_img1 = argv[2];
            sigma = atof(argv[3]);
            
            // TODO: Xin, add your code here
            
        }
        else if (!strcmp(argv[1], "-o") || !strcmp(argv[1], "-op")){
            printf("Outline Detection in process ...\n");
            if (argc != 3){
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
                // TODO: Michael, add your sequential code here
                // TODO: Haokun, add your sequential code here
                // TODO: Mengyi, add your sequential code here
                // TODO: Xin, add your sequential code here
                // TODO: Ali, add your sequential code here
            }
            else if (argc == 7 && !strcmp(argv[1], "-p")){
                // assign parameters
                input_img1 = argv[2];
                input_img2 = argv[3];
                deg = atof(argv[4]);
                scale_fac = atof(argv[5]);
                sigma = atof(argv[6]);
                
                // TODO: Ding, add your parallel code here
                // TODO: Michael, add your parallel code here
                // TODO: Haokun, add your parallel code here
                // TODO: Mengyi, add your parallel code here
                // TODO: Xin, add your parallel code here
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
