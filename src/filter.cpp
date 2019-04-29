#include <malloc.h>
#include <math.h>
#include <assert.h>

#define M_PI 3.14159265358979323846

void make_tri_filter(int r, float** filter_array)
{
	float nrm = 0, v;
	int sz = 2 * r + 1, i;
	if (filter_array == NULL) assert(0);
	*filter_array = (float*)malloc(sz * sizeof(float));
	float* filter = *filter_array;
	for (i = 0; i < r; ++i) {
		v = (float)i + 1;
		nrm += 2 * v;
		(filter)[i] = (filter)[sz - i - 1] = v;
	}
	v = (float)r + 1;
	(filter)[r] = v;
	nrm += v;
	nrm = 1.0f / nrm;
	for (i = 0; i < sz; ++i) {
		(filter)[i] *= nrm;
	}
}

void make_gaussian_filter(float sigma, int width, int height, float** filter_array) {

	assert(width % 2 == 1);
	assert(height % 2 == 1);

	if (filter_array == NULL) assert(0);
	*filter_array = (float*)malloc(width*height * sizeof(float));
	float* filter = *filter_array;
	float* filter_line = filter;
	float R;
	int ctr_x = (width - 1) / 2;
	int	ctr_y = (height - 1) / 2;
	float sum = 0;
	float var = 2 * sigma*sigma;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			R = (ctr_x - j)*(ctr_x - j) + (ctr_y - i)*(ctr_y - i);
			filter_line[j] = 1 / (var*M_PI)*exp(-R / var);
			sum += filter_line[j];
		}
		filter_line += width;
	}

	filter_line = filter;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			filter_line[j]/=sum;
		}
		filter_line += width;
	}

}
