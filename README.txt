CS133_group_project
===================

Six fantastic image operations in parallel implementation

Compile the project
===================
    gcc -fopenmp -lm -o imgproj IO/bmpfile.c main.c

Scaling
===================
    All the example img files are in the scaling_samples folder
    test.bmp ~ test4.bmp are the files I used to calculate time difference
    based on different input size.

    large.bmp is a large input file I used to calculate time difference
    based on different thread numbers.
    Command:
        ./imgproj -s img_name scale_factor (single thread)
        ./imgproj -sp img_name scale_factor thread_num (multi thread)


    Exam multithreading on large input file:
       time ./imgproj -s scaling_samples/larg.bmp 2 (no parallelization)
       time ./imgproj -sp scaling_samples/larg.bmp 2 16 (16 threads)
