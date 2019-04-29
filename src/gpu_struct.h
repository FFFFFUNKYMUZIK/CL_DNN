#pragma once

#include <CL/cl.h>


typedef struct gpu_test_program_s {
	cl_program test_program;	
	cl_kernel test_kernel;
} gpu_test_program;

typedef struct gpu_mem_s {
	cl_mem src;
	cl_mem dst;
} gpu_mem;

typedef struct gpu_struct_s {
	cl_context ctx;
	cl_command_queue command_queue;
	cl_device_id deviceIDs[1];
	gpu_test_program test_prog;
	gpu_mem cl_memory;
} gpu_struct;

extern gpu_struct* pgpust;

void init();
void deinit();