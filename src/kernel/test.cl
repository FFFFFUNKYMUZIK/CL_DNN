__kernel void test_kernel(__global const uchar* src, __global uchar* dst,
				      __global const float* filter, 
					  const int2 size, 
					  const int2 filter_size){
	int x = get_global_id(0);
	int y = get_global_id(1);
	
	int filter_startx = 0;
	int filter_starty = 0;

	int w = size.s0;
	int h = size.s1;

	if (x<(filter_size.s0/2)){
		filter_startx = filter_size.s0/2 - x;
	}
	if (y<(filter_size.s1/2)){
		filter_starty = filter_size.s1/2 - y;
	}

	float sum = 0;
	int x_start = x-filter_size.s0/2;
	int y_start = y-filter_size.s1/2;

	for ( int i = filter_starty ; i<filter_size.s1 ; i++){
		for (int j = filter_startx ; j<filter_size.s0 ; j++){
			sum += filter[filter_size.s0 * i + j] * src[ w * (y_start+i) + (x_start+j)];
		}
	}
	
	if (sum>255) sum=255;
	if (sum<0) sum=0;

	dst[w * y +  x] = (uchar)sum;
}
