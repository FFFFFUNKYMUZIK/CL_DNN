__kernel void convolution(__global const char* src, __global const char* dst,
				      __global const char* filter, cl_int2 size, cl_int2 dst_size, cl_int2 filter_size, cl_int2 padding_size){
	int x = get_global_id(0);
	int y = get_global_id(1);
	
	int filter_startx = 0;
	int filter_starty = 0;

	int w = size.s0;
	int h = size.s1;
	
	if (((padding_size.s0+x)<(filter_size.s0-1))||(padding.size_s1+y)<(filter_size.s1-1)){
		return;
	}
	
	if (x<(filter_size.s0/2)){
		filter_startx = filter_size.s0/2 - x;
	}
	if (y<(filter_size.s1/2)){
		filter_starty = filter_size.s1/2 - y;
	}

	int sum = 0;
	int x_start = x-filter_size.s0/2;
	int y_start = y-filter_size.s1/2;

	for (int i = filter_startx ; i<filter_size.s0 ; i++){
		for ( int j = filter_starty ; j<filter_size.s1 ; j++){
			sum += filter[filter_size.s0 * j + i] * src[ w * (y_start+j) + (x_start+i)];
		}
	}

	int dst_w = size.s0 + 2*padding_size.s0-2*(filter.size.s0/2);

//	dst[dst_w *  +  ] = sum;
}
