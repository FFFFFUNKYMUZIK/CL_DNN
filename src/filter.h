#pragma once

void make_tri_filter(int r, float** filter);
void make_gaussian_filter(float sigma, int width, int height, float** filter_array);