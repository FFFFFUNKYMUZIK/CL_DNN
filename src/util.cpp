#include "util.h"
#include <string.h>
#include <stdlib.h>


char const *clGetErrorString(cl_int const err)
{
	switch (err)
	{
		CL_ERR_TO_STR(CL_SUCCESS);
		CL_ERR_TO_STR(CL_DEVICE_NOT_FOUND);
		CL_ERR_TO_STR(CL_DEVICE_NOT_AVAILABLE);
		CL_ERR_TO_STR(CL_COMPILER_NOT_AVAILABLE);
		CL_ERR_TO_STR(CL_MEM_OBJECT_ALLOCATION_FAILURE);
		CL_ERR_TO_STR(CL_OUT_OF_RESOURCES);
		CL_ERR_TO_STR(CL_OUT_OF_HOST_MEMORY);
		CL_ERR_TO_STR(CL_PROFILING_INFO_NOT_AVAILABLE);
		CL_ERR_TO_STR(CL_MEM_COPY_OVERLAP);
		CL_ERR_TO_STR(CL_IMAGE_FORMAT_MISMATCH);
		CL_ERR_TO_STR(CL_IMAGE_FORMAT_NOT_SUPPORTED);
		CL_ERR_TO_STR(CL_BUILD_PROGRAM_FAILURE);
		CL_ERR_TO_STR(CL_MAP_FAILURE);
		CL_ERR_TO_STR(CL_MISALIGNED_SUB_BUFFER_OFFSET);
		CL_ERR_TO_STR(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
		CL_ERR_TO_STR(CL_COMPILE_PROGRAM_FAILURE);
		CL_ERR_TO_STR(CL_LINKER_NOT_AVAILABLE);
		CL_ERR_TO_STR(CL_LINK_PROGRAM_FAILURE);
		CL_ERR_TO_STR(CL_DEVICE_PARTITION_FAILED);
		CL_ERR_TO_STR(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
		CL_ERR_TO_STR(CL_INVALID_VALUE);
		CL_ERR_TO_STR(CL_INVALID_DEVICE_TYPE);
		CL_ERR_TO_STR(CL_INVALID_PLATFORM);
		CL_ERR_TO_STR(CL_INVALID_DEVICE);
		CL_ERR_TO_STR(CL_INVALID_CONTEXT);
		CL_ERR_TO_STR(CL_INVALID_QUEUE_PROPERTIES);
		CL_ERR_TO_STR(CL_INVALID_COMMAND_QUEUE);
		CL_ERR_TO_STR(CL_INVALID_HOST_PTR);
		CL_ERR_TO_STR(CL_INVALID_MEM_OBJECT);
		CL_ERR_TO_STR(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
		CL_ERR_TO_STR(CL_INVALID_IMAGE_SIZE);
		CL_ERR_TO_STR(CL_INVALID_SAMPLER);
		CL_ERR_TO_STR(CL_INVALID_BINARY);
		CL_ERR_TO_STR(CL_INVALID_BUILD_OPTIONS);
		CL_ERR_TO_STR(CL_INVALID_PROGRAM);
		CL_ERR_TO_STR(CL_INVALID_PROGRAM_EXECUTABLE);
		CL_ERR_TO_STR(CL_INVALID_KERNEL_NAME);
		CL_ERR_TO_STR(CL_INVALID_KERNEL_DEFINITION);
		CL_ERR_TO_STR(CL_INVALID_KERNEL);
		CL_ERR_TO_STR(CL_INVALID_ARG_INDEX);
		CL_ERR_TO_STR(CL_INVALID_ARG_VALUE);
		CL_ERR_TO_STR(CL_INVALID_ARG_SIZE);
		CL_ERR_TO_STR(CL_INVALID_KERNEL_ARGS);
		CL_ERR_TO_STR(CL_INVALID_WORK_DIMENSION);
		CL_ERR_TO_STR(CL_INVALID_WORK_GROUP_SIZE);
		CL_ERR_TO_STR(CL_INVALID_WORK_ITEM_SIZE);
		CL_ERR_TO_STR(CL_INVALID_GLOBAL_OFFSET);
		CL_ERR_TO_STR(CL_INVALID_EVENT_WAIT_LIST);
		CL_ERR_TO_STR(CL_INVALID_EVENT);
		CL_ERR_TO_STR(CL_INVALID_OPERATION);
		CL_ERR_TO_STR(CL_INVALID_GL_OBJECT);
		CL_ERR_TO_STR(CL_INVALID_BUFFER_SIZE);
		CL_ERR_TO_STR(CL_INVALID_MIP_LEVEL);
		CL_ERR_TO_STR(CL_INVALID_GLOBAL_WORK_SIZE);
		CL_ERR_TO_STR(CL_INVALID_PROPERTY);
		CL_ERR_TO_STR(CL_INVALID_IMAGE_DESCRIPTOR);
		CL_ERR_TO_STR(CL_INVALID_COMPILER_OPTIONS);
		CL_ERR_TO_STR(CL_INVALID_LINKER_OPTIONS);
		CL_ERR_TO_STR(CL_INVALID_DEVICE_PARTITION_COUNT);
		CL_ERR_TO_STR(CL_INVALID_PIPE_SIZE);
		CL_ERR_TO_STR(CL_INVALID_DEVICE_QUEUE);

	default:
		return "UNKNOWN ERROR CODE";
	}
}



void read_program(char * c, unsigned int maxcount, const char * filename, bool print_flag) {

	FILE * fp = NULL;
	fp = fopen(filename, "rb");
	char temp[1024];
	unsigned int strpos = 0;

	int i = 0;
	if (fp != NULL)
	{
		printf("\nRead Kernel : %s\n", filename);
	}
	else
	{
		printf("\nRead Kernel failed : %s\n", filename);
		return;
	}

	while (!feof(fp))
	{
		char * a;
		a = fgets(temp, 1024, fp);

		if (a == NULL) break;
		if (print_flag) printf("%s", temp);
		strcpy(c + strpos, temp);
		strpos += strlen(temp);
		i++;
		if (i >= maxcount) break;
	}
	fclose(fp);

	return;
}


cl_program build_program(gpu_struct * pgpust, const char * src)
{
	cl_int err = CL_SUCCESS;
	cl_program p = NULL;
	const char * program_string[] = { src };
	p = clCreateProgramWithSource(pgpust->ctx, 1, program_string, NULL, &err);
	printerr(err);

	printf("building program... \n");
	err = clBuildProgram(p, 1, &pgpust->deviceIDs[0], NULL, NULL, NULL);
	//print build error log if failed
	if (err != CL_BUILD_SUCCESS) {
		//get log len
		size_t len;
		err = clGetProgramBuildInfo(p, pgpust->deviceIDs[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
		printerr(err);
		//get log
		char* log = (char*)malloc(len);
		err = clGetProgramBuildInfo(p, pgpust->deviceIDs[0], CL_PROGRAM_BUILD_LOG, len, log, NULL);
		printerr(err);
		printf("%s\n", log);
		free(log);
	}
	return p;
}

cl_mem create_buffer(gpu_struct * pgpust, unsigned char * host_ptr, unsigned int length, int flag) {
	cl_int err = CL_SUCCESS;
	assert(length);
	cl_mem buf = clCreateBuffer(pgpust->ctx, flag, length, host_ptr, &err);
	printerr(err);

	return buf;
}

void release_buffer(cl_mem buf) {
	cl_int err = CL_SUCCESS;
	assert(buf);
	err = clReleaseMemObject(buf);
	printerr(err);
	return;
}