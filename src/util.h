#pragma once
#include "gpu_struct.h"
#include <CL/cl.h>
#include <stdio.h>
#include <assert.h>

#define STR_BUFF 10000
#define MAX_LINE 1000
#define PRINT_FLAG false

#define CL_ERR_TO_STR(err) case err: return #err
char const * clGetErrorString(cl_int const err);

#ifdef _DEBUG
#define printerr(err) if (err != CL_SUCCESS){						\
	printf("[%d] CL error : %s \n", err, clGetErrorString(err));	\
	assert(err==CL_SUCCESS);}										\

#else
#define printerr(err) assert(err==CL_SUCCESS)
#endif

void read_program(char * c, unsigned int maxcount, const char * filename, bool printflag);
cl_program build_program(gpu_struct * pgpust, const char * src);
cl_mem create_buffer(gpu_struct * pgpust, unsigned char * host_ptr, unsigned int length, int flag);
void release_buffer(cl_mem buf);
