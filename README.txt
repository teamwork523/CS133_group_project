CS133_group_project
===================

Six fantastic image operations in parallel implementation

Compile the project
===================
    gcc -fopenmp -lm -o imgproj IO/bmpfile.c main.c

File Location
===================
	Source Code:  All source codes are under "src" folder
	Input Images: All input images are under "img" folder.
				  Please use all images under the subfolders with operation 
				  names to test our project.
	Executable:   Execuatable file is under "src" folder after
	              you compile the project.

Motion estimation
===================
	Motion operation takes in two input image path. For parallelism,
	you can specify the nubmer of threadsas parameters.
	Notice that the size of the two input image must be the same, 
	and the length and the height of the image must be dividable by 16.

	Command:
		./imgproj -m  img_path1 img_path2
		./imgproj -mp img_path1 img_path2 (N_threads)
		# Notice that N_threads is optional
	
	The output image is under "src" folder, named "bmp_out.bmp".
	There is an additional "output.txt" file containing motion vectors.

// TODO: Michael, add your part here

Rotation
===================
	Rotation operation takes in one input image path (either
	relative or absolute) and a rotation degree. For parallelism,
	you can specify the nubmer of threads and chunk size as
	parameters.

	Command:
		./imgproj -r  img_name degree
		./imgproj -rp img_name degree (N_threads N_chunk)
		# Notice that N_threads and N_chunk are optional
	
	If you run the sequential version, you will get an output
	image under "src" folder, named "bmp_rotation_seq.bmp".
	If you run the parallel version, you will get an output
	image under "src" folder, named "bmp_rotation_parallel.bmp"

Scaling
===================
    Scaling operation takes in a input image and a scale_factor
    and output a scaled image named scaling.bmp.
    Command:
        ./imgproj -s img_name scale_factor (single thread)
        ./imgproj -sp img_name scale_factor thread_num (multi thread)

    All the example img files are in the scaling_samples folder
    test.bmp ~ test4.bmp are the files I used to calculate time 
    difference based on different input size.large.bmp is a large 
    input file I used to calculate time difference based on different 
    thread numbers.

    Exam multithreading on large input file:
       time ./imgproj -s scaling_samples/larg.bmp 2 (no parallelization)
       time ./imgproj -sp scaling_samples/larg.bmp 2 16 (16 threads)

Gaussian Blur
===================
	Gaussian Blur operation takes in one input image path (either
	relative or absolute) and a sigma value. For parallelism,
	you can specify the nubmer of threads and chunk size as
	parameters.

	Command:
    	./imgproj -g  img_name sigma
		./imgproj -gp img_name sigma (N_threads N_chunk)
         # Notice that N_threads and N_chunk are optional

	If you run the sequential version, you will get an output
	image under "src" folder, named "gaussian_blur_seq.bmp".
	If you run the parallel version, you will get an output
	image under "src" folder, named "gaussian_blur_parallel.bmp"

// TODO: Ali, add your part here
