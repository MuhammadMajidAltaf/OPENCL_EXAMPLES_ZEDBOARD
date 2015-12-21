#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "pgm.h"

#define MAX_SOURCE_SIZE (0x100000)

cl_device_id device_id = NULL;
cl_context context = NULL;
cl_command_queue queue = NULL;
cl_program program = NULL;

 
int setWorkSize(size_t* gws, size_t* lws, cl_int x, cl_int y)
{
    switch(y) {
        case 1:
            gws[0] = x;
            gws[1] = 1;
            lws[0] = 128;
            lws[1] = 1;
            break;
        default:
            gws[0] = x;
            gws[1] = y;
            lws[0] = 16;
            lws[1] = 8;
            break;
    }

    return 0;
}


int main()
{
    
    cl_mem xmobj = NULL;
    cl_mem rmobj = NULL;
    cl_kernel trns = NULL;

    cl_platform_id platform_id = NULL;

    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;

    cl_int ret;

	cl_float *xm;
	cl_float *rm;
	
    pgm_t ipgm;
    pgm_t opgm;

    FILE *fp;
    const char fileName[] = "./test2.cl";
    size_t source_size;
    char *source_str;
    cl_int i, j;
    cl_int n;

    size_t gws[2];
    size_t lws[2];


    
	/*  Load source code , including kernel */
	fp = fopen(fileName, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char *)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp );
    fclose( fp );

	/* Image file input */
	readPGM(&ipgm, "lena.pgm");

    n = ipgm.width; 

    xm = (cl_float *)malloc(n * n * sizeof(cl_float));
	rm = (cl_float *)malloc(n * n * sizeof(cl_float));

    for( i = 0; i < n; i++ ) {
        for( j = 0; j < n; j++ ) {
			((float*)xm)[(n*j) + i] = (float)ipgm.buf[n*j + i];

        }
    }

	/* Acquisition of platform devices information */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	/* OpenCL Creating a Context */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	/* Creating a command queue */
	queue = clCreateCommandQueue(context, device_id, 0, &ret);

	/* Creating a buffer object */
	xmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, n*n*sizeof(cl_float), NULL, &ret);
	rmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, n*n*sizeof(cl_float), NULL, &ret);


	/* And transfer the data to the memory buffer */
	ret = clEnqueueWriteBuffer(queue, xmobj, CL_TRUE, 0, n*n*sizeof(cl_float), xm, 0, NULL, NULL);


	/* Create a kernel program from the read source */
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

	/* Build a kernel program */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	/* OpenCL Creating the kernel */
	trns = clCreateKernel(program, "transpose", &ret);

  
	/* set kernel arguments */
	ret = clSetKernelArg(trns, 0, sizeof(cl_mem), (void *)&rmobj);
    ret = clSetKernelArg(trns, 1, sizeof(cl_mem), (void *)&xmobj);
	ret = clSetKernelArg(trns, 2, sizeof(cl_int), (void *)&n);

    //setWorkSize(gws, lws, n, n);
	gws[0] = n;
	gws[1] = n;

	/*Enque task for parallel execution*/
	ret = clEnqueueNDRangeKernel(queue, trns, 2, NULL, gws, NULL, 0, NULL, NULL);

	/* Get results from the memory buffer */
	ret = clEnqueueReadBuffer(queue, rmobj, CL_TRUE, 0, n*n*sizeof(cl_float), rm, 0, NULL, NULL);

	
	/* Copy to buffer */
	float* ampd;
    ampd = (float*)malloc(n*n*sizeof(float));
    for (i=0; i<n; i++) {
        for (j=0; j<n; j++) {
			ampd[n*((i)) + ((j))] = (((float*)rm)[(n*i) + j]);
        }
    }
    opgm.width = n;
    opgm.height = n;
    normalizeF2PGM(&opgm, ampd);
    free(ampd);

	/* Image file output */
	writePGM(&opgm, "output.pgm");


	/*End processing */
	ret = clFlush(queue);
    ret = clFinish(queue);
    ret = clReleaseKernel(trns);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(xmobj);
    ret = clReleaseMemObject(rmobj);
    ret = clReleaseCommandQueue(queue);
    ret = clReleaseContext(context);

    destroyPGM(&ipgm);
    destroyPGM(&opgm);

    free(source_str);
    free(rm);
    free(xm);

    return 0;
}
