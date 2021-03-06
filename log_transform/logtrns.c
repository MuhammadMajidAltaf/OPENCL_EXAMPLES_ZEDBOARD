#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "pgm.h"
#include "papi.h"

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
	printf("cl:main program:log_transform\n");
	cl_event event;

        long long ptimer1=0;
        long long ptimer2=0;

	long long ptotal_start = 0;
	long long ptotal_end = 0;

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
    const char fileName[] = "./logtrns.cl";
    size_t source_size;
    char *source_str;
    cl_int i, j, width, height;

    size_t gws[2];
    size_t lws[2];


   /*Read kernel */ 
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

	width = ipgm.width;
	height = ipgm.height;
	printf("width of image is %d\n", width);
	printf("width of image is %d\n", height);

	xm = (cl_float *)malloc(width * height * sizeof(cl_float));
	rm = (cl_float *)malloc(width * height * sizeof(cl_float));

	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			((float*)xm)[(width*j) + i] = (float)ipgm.buf[width*j + i];

        	}
    	}	
     printf("cl:main program:Init_Device \n");

     ptimer1 = PAPI_get_virt_usec();
    /* Get platform and device information*/
    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ptimer2 = PAPI_get_virt_usec();
    printf("cl:main timing:PAPI clGetPlatformIDs %llu us\n",(ptimer2-ptimer1));


    ptimer1 = PAPI_get_virt_usec();
    ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);
    ptimer2 = PAPI_get_virt_usec();
    printf("cl:main timing:PAPI clGetDeviceIDs %llu us\n",(ptimer2-ptimer1));


    ptimer1 = PAPI_get_virt_usec();
    /* OpenCL create context */
    context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
    ptimer2 = PAPI_get_virt_usec();
    printf("cl:main timing:PAPI clCreateContext %llu us\n",(ptimer2-ptimer1));


    ptimer1 = PAPI_get_virt_usec();
    /* Create command queue */
    queue = clCreateCommandQueue(context, device_id,  CL_QUEUE_PROFILING_ENABLE, &ret);
    ptimer2 = PAPI_get_virt_usec();
    printf("cl:main timing:PAPI clCreateCommandQueue %llu us\n",(ptimer2-ptimer1));


    ptimer1 = PAPI_get_virt_usec();
    /* Create memory buffer */
    xmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, width*height*sizeof(cl_float), NULL, &ret);
    rmobj = clCreateBuffer(context, CL_MEM_READ_WRITE, width*height*sizeof(cl_float), NULL, &ret);
    ptimer2 = PAPI_get_virt_usec();
    printf("cl:main timing:PAPI clCreateBuffer %llu us\n",(ptimer2-ptimer1));


    ptimer1 = PAPI_get_virt_usec();
    /* Copy to memory buffer */
    ret = clEnqueueWriteBuffer(queue, xmobj, CL_TRUE, 0, width*height*sizeof(cl_float), xm, 0, NULL, NULL);
    ptimer2 = PAPI_get_virt_usec();
    printf("cl:main timing:PAPI clEnqueueWriteBuffer %llu us\n",(ptimer2-ptimer1));


    ptimer1 = PAPI_get_virt_usec();
    /*Create kernel program from read source code */
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    ptimer2 = PAPI_get_virt_usec();
    printf("cl:main timing:PAPI clCreateProgramWithSource %llu us\n",(ptimer2-ptimer1));


    ptimer1 = PAPI_get_virt_usec();
    /* Build kernel code*/ 
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    ptimer2 = PAPI_get_virt_usec();
    printf("cl:main timing:PAPI clBuildProgram %llu us\n",(ptimer2-ptimer1));


    ptimer1 = PAPI_get_virt_usec();
    /* OpenCL creating kernel*/
    trns = clCreateKernel(program, "logtrns",    &ret);
    ptimer2 = PAPI_get_virt_usec();
    printf("cl:main timing:PAPI clCreateKernel %llu us\n",(ptimer2-ptimer1));


    ptimer1 = PAPI_get_virt_usec();  
    /* Set kernel arguments */
    ret = clSetKernelArg(trns, 0, sizeof(cl_mem), (void *)&rmobj);
    ret = clSetKernelArg(trns, 1, sizeof(cl_mem), (void *)&xmobj);
    ret = clSetKernelArg(trns, 2, sizeof(cl_int), (void *)&width);
    ptimer2 = PAPI_get_virt_usec();
    printf("cl:main timing:PAPI clSetKernelArg %llu us\n",(ptimer2-ptimer1));

    gws[0] = width;
    gws[1] = height;
	//can also use setworksize() function

    ptimer1 = PAPI_get_virt_usec();
    /*Enque task for parallel execution*/
    ret = clEnqueueNDRangeKernel(queue, trns, 2, NULL, gws, NULL, 0, NULL, &event);
    ptimer2 = PAPI_get_virt_usec();
    printf("cl:main timing:PAPI clEnqueueNDRangeKernel %llu us\n",(ptimer2-ptimer1));


	//opencl timer
        clWaitForEvents(1, &event);
        clFinish(queue);
        cl_ulong time_start, time_end;
        double total_time;
        clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
        clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
        total_time = time_end - time_start;
        printf("cl:main timing:opencl clEnqueueNDRangeKernel: %0.3f us \n", total_time / 1000.0);

    ptimer1 = PAPI_get_virt_usec();
    /* Get result from memory buffer */
    ret = clEnqueueReadBuffer(queue, rmobj, CL_TRUE, 0, width*height*sizeof(cl_float), rm, 0, NULL, NULL);
    ptotal_end = PAPI_get_virt_usec();
    ptimer2 = PAPI_get_virt_usec();
    printf("cl:main timing:PAPI clEnqueueReadBuffer %llu us\n",(ptimer2-ptimer1));

    printf("cl:main timing:PAPI total_time %llu us\n",(ptotal_end-ptotal_start));
	
    opgm.width = width;
    opgm.height = height;
    normalizeF2PGM(&opgm, rm);

    /* Output image */
    writePGM(&opgm, "output.pgm");


    /* End processing */
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
