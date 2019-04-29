#include "tester.h"
#include "util.h"
#include <malloc.h>
#include "filter.h"

#define FILTER_NUM 1
#define RADIUS 5
#define GAUSSIAN_WIDTH 9
#define GAUSSIAN_HEIGHT 9
const char test_program_file[] = "./kernel/test.cl";
float* tri_filter[FILTER_NUM];
cl_mem cl_filter;

void tester_init() {

	cl_int err = CL_SUCCESS;
	gpu_test_program* program_struct = &pgpust->test_prog;
	char * test_program_string = new char[STR_BUFF];

	//read cl program
	read_program(test_program_string, MAX_LINE, test_program_file, PRINT_FLAG);
	//build cl program
	program_struct->test_program = build_program(pgpust, test_program_string);
	delete test_program_string;

	program_struct->test_kernel = clCreateKernel(program_struct->test_program, "test_kernel", &err);
	printerr(err);

	pgpust->cl_memory.src = create_buffer(pgpust, NULL, 1920 * 1080 * sizeof(char), CL_MEM_READ_WRITE);
	pgpust->cl_memory.dst = create_buffer(pgpust, NULL, 1920 * 1080 * sizeof(char), CL_MEM_READ_WRITE);
	
	//custom init
	//////////////////////////////////////////////////////////////////////////////////////////

	make_gaussian_filter(100, GAUSSIAN_WIDTH, GAUSSIAN_HEIGHT, tri_filter);
	cl_filter = create_buffer(pgpust, (unsigned char*) tri_filter[0], sizeof(float)*(GAUSSIAN_WIDTH *  GAUSSIAN_HEIGHT), CL_MEM_ALLOC_HOST_PTR | CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY);
	//printf("%lf %lf %lf %lf %lf \n", tri_filter[0][0], tri_filter[0][1], tri_filter[0][2], tri_filter[0][3], tri_filter[0][4]);
	//printf("%lf %lf %lf %lf %lf \n", tri_filter[0][5], tri_filter[0][6], tri_filter[0][7], tri_filter[0][8], tri_filter[0][9]);
	//printf("%lf %lf %lf %lf %lf \n", tri_filter[0][10], tri_filter[0][11], tri_filter[0][12], tri_filter[0][13], tri_filter[0][14]);

	//////////////////////////////////////////////////////////////////////////////////////////
}

void tester_deinit() {
	cl_int err = CL_SUCCESS;
	gpu_test_program * program_struct = &pgpust->test_prog;
	

	//custom deinit
	//////////////////////////////////////////////////////////////////////////////////////////

	free(tri_filter[0]);
	release_buffer(cl_filter);

	//////////////////////////////////////////////////////////////////////////////////////////

	release_buffer(pgpust->cl_memory.src);
	release_buffer(pgpust->cl_memory.dst);

	err = clReleaseKernel(program_struct->test_kernel);
	printerr(err);
	err = clReleaseProgram(program_struct->test_program);
	printerr(err);
	}

void run(unsigned char * src, unsigned char * dst, int width, int height) {
	
	cl_int err = CL_SUCCESS;

	gpu_test_program* program_struct = &pgpust->test_prog;
	
	//Write source buf
	err = clEnqueueWriteBuffer(pgpust->command_queue, pgpust->cl_memory.src, CL_TRUE, 0, (size_t)width*height, src, 0, NULL, NULL);
	printerr(err);

	// src&dst buffer
	err = clSetKernelArg(program_struct->test_kernel, 0, sizeof(cl_mem), &pgpust->cl_memory.src);
	printerr(err);
	err = clSetKernelArg(program_struct->test_kernel, 1, sizeof(cl_mem), &pgpust->cl_memory.dst);
	printerr(err);

	//custom inputs
	//////////////////////////////////////////////////////////////////////////////////////////

	err = clSetKernelArg(program_struct->test_kernel, 2, sizeof(cl_mem), &cl_filter);
	printerr(err);
	
	Size2D image_size;
	image_size.w = width;
	image_size.h = height;
	Size2D filter_size;
	filter_size.w = GAUSSIAN_WIDTH;
	filter_size.h = GAUSSIAN_HEIGHT;

	err = clSetKernelArg(program_struct->test_kernel, 3, sizeof(cl_int2), &image_size);
	printerr(err);

	err = clSetKernelArg(program_struct->test_kernel, 4, sizeof(cl_int2), &filter_size);
	printerr(err);

	//////////////////////////////////////////////////////////////////////////////////////////

	
	const size_t size[] = { width, height, 1 };
	const size_t offset[] = { 0, 0, 0 };
	err = clEnqueueNDRangeKernel(pgpust->command_queue, program_struct->test_kernel, 2, offset, size, NULL, 0, NULL, NULL);
	printerr(err);

	//Read dst buf
	err = clEnqueueReadBuffer(pgpust->command_queue, pgpust->cl_memory.dst, CL_TRUE, 0, (size_t)width*height, dst, 0, NULL, NULL);
	printerr(err);

	clFinish(pgpust->command_queue);

}