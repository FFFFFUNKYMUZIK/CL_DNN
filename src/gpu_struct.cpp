#include "gpu_struct.h"
#include <stdlib.h>
#include "util.h"
#include "tester.h"

gpu_struct* pgpust = NULL;

const char* attributeNames[5] = { "Name", "Vendor", "Version", "Profile", "Extensions" };
const cl_platform_info attributeTypes[5] = { CL_PLATFORM_NAME,
CL_PLATFORM_VENDOR,
CL_PLATFORM_VERSION,
CL_PLATFORM_PROFILE,
CL_PLATFORM_EXTENSIONS };

static void create(gpu_struct** ppgpust) {

	cl_int err = CL_SUCCESS;
	cl_platform_id* pPlatformIDs = NULL;
	cl_device_id* pDeviceIDs = NULL;
	cl_context pContext = NULL;
	cl_uint numPlatform = 0, numDevice = 0;

	*ppgpust = (gpu_struct*)malloc(sizeof(gpu_struct));

	// query platform number
	err = clGetPlatformIDs(0, NULL, &numPlatform);
	printerr(err);
	if (numPlatform == 0) {
		printf("no available platform! \n");
		return;
	}

	// query platform IDs
	pPlatformIDs = new cl_platform_id[numPlatform];
	err = clGetPlatformIDs(numPlatform, pPlatformIDs, NULL);
	printerr(err);

	//query platform info
	for (int i = 0; i<numPlatform; i++) {
		printf("[%d] th platform info : \n", i);
		for (int j = 0; j<5; j++) {
			size_t infoSize;
			char * info;
			err = clGetPlatformInfo(pPlatformIDs[i], attributeTypes[j], 0, NULL, &infoSize);
			printerr(err);
			info = (char*)malloc(infoSize);

			err = clGetPlatformInfo(pPlatformIDs[i], attributeTypes[j], infoSize, info, NULL);
			printerr(err);
			printf(" ---attr[%d] %s : %s\n", j, attributeNames[j], info);
			free(info);
		}
		printf("\n");

	}


	//select platform to use
	cl_platform_id usePlatform = pPlatformIDs[0];

	// query device number
	err = clGetDeviceIDs(usePlatform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevice);
	printerr(err);
	if (numDevice == 0) {
		printf("no available device! \n");
		return;
	}

	// query device IDs
	pDeviceIDs = (*ppgpust)->deviceIDs;
	err = clGetDeviceIDs(usePlatform, CL_DEVICE_TYPE_GPU, numDevice, pDeviceIDs, NULL);
	printerr(err);

	// set context properties
	cl_context_properties props[] =
	{
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)usePlatform,
		0
	};

	delete(pPlatformIDs);

	// create context
	(*ppgpust)->ctx = clCreateContext(props, numDevice, pDeviceIDs, NULL, NULL, &err);
	printerr(err);
	if ((*ppgpust)->ctx == 0) {
		printf("context creation fail! \n");
		return;
	}

	//select device to use
	cl_device_id useDevice = pDeviceIDs[0];

	//query device info
	for (int i = 0; i<numDevice; i++) {
		printf("[%d] th device info : \n", i);
		size_t infoSize;
		char * info = (char *)malloc(100);

		//query device type
		cl_device_type device_type;
		clGetDeviceInfo(pDeviceIDs[i], CL_DEVICE_TYPE, 0, NULL, &infoSize);
		clGetDeviceInfo(pDeviceIDs[i], CL_DEVICE_TYPE, infoSize, &device_type, NULL);
		char * device_type_arr[] = { "DEFAULT", "CPU", "GPU", "ACC", "CUS" };
		char device_index = 0;

		if (device_type&CL_DEVICE_TYPE_CPU) device_index = 1;
		else if (device_type&CL_DEVICE_TYPE_GPU) device_index = 2;
		else if (device_type&CL_DEVICE_TYPE_ACCELERATOR) device_index = 3;
		else if (device_type&CL_DEVICE_TYPE_CUSTOM) device_index = 4;
		printf("---Device type : %s\n", device_type_arr[device_index]);

		//#define CL_DEVICE_TYPE_DEFAULT                      (1 << 0)
		//#define CL_DEVICE_TYPE_CPU                          (1 << 1)
		//#define CL_DEVICE_TYPE_GPU                          (1 << 2)
		//#define CL_DEVICE_TYPE_ACCELERATOR                  (1 << 3)
		//#define CL_DEVICE_TYPE_CUSTOM                       (1 << 4)
		//#define CL_DEVICE_TYPE_ALL                          0xFFFFFFFF

		//query device vendor
		clGetDeviceInfo(pDeviceIDs[i], CL_DEVICE_VENDOR, 0, NULL, &infoSize);
		clGetDeviceInfo(pDeviceIDs[i], CL_DEVICE_VENDOR, infoSize, info, NULL);
		printf("---Device vendor : %s\n", info);

		//query device vendor id
		cl_uint a;
		clGetDeviceInfo(pDeviceIDs[i], CL_DEVICE_VENDOR_ID, 0, NULL, &infoSize);
		clGetDeviceInfo(pDeviceIDs[i], CL_DEVICE_VENDOR_ID, infoSize, &a, NULL);
		printf("---Device vendor ID : %d\n", a);

		//query device version
		clGetDeviceInfo(pDeviceIDs[i], CL_DEVICE_VERSION, 0, NULL, &infoSize);
		clGetDeviceInfo(pDeviceIDs[i], CL_DEVICE_VERSION, infoSize, info, NULL);
		printf("---Device version : %s\n", info);

		//query driver version				
		clGetDeviceInfo(pDeviceIDs[i], CL_DRIVER_VERSION, 0, NULL, &infoSize);
		clGetDeviceInfo(pDeviceIDs[i], CL_DRIVER_VERSION, infoSize, info, NULL);
		printf("---Driver version : %s\n", info);

		printf("\n");
		free(info);

	}

	// create command queue
	(*ppgpust)->command_queue = clCreateCommandQueue((*ppgpust)->ctx, useDevice, 0, &err);
	printerr(err);
	if ((*ppgpust)->command_queue == 0) {
		printf("command queue creation fail! \n");
		return;
	}

}

static void release(gpu_struct* pgpust) {
	clFinish(pgpust->command_queue);
	clReleaseCommandQueue(pgpust->command_queue);
	clReleaseContext(pgpust->ctx);
	free(pgpust);
};

static void sync_gpu(gpu_struct* pgpust) {
	//wait for doing all CL command in command queue
	clFinish(pgpust->command_queue);
}


void init() {
	if (pgpust == NULL) {
		create(&pgpust);
	}

	tester_init();
}

void deinit() {
	if (pgpust != NULL) {
		release(pgpust);
	}

	tester_deinit();
}