#pragma once

#include "gpu_struct.h"
#include "types.h"

void tester_init();
void tester_deinit();
void run(unsigned char * src, unsigned char * dst, int width, int height);