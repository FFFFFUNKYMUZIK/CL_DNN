__kernel void rgba8uc4_from_yuv420_888_crop(
    const uint YRowstride, const uint YPixelstride,
	const uint URowstride, const uint UPixelstride,
	const uint VRowstride, const uint VPixelstride,
	const uint crop_x1, const uint crop_y1, const int dst_w, 
	__global uchar* Y, __global uchar* U, __global uchar* V,
	__global uchar4* dst)
{
    const int x_tgt = get_global_id(0);
	const int y_tgt = get_global_id(1);
	const int x = x_tgt + crop_x1;
	const int y = y_tgt + crop_y1;
	int y_c, u_c, v_c;
		
	y_c = Y[YRowstride * y + YPixelstride * x]-16;
	u_c = U[URowstride * (y>>1) + UPixelstride * (x>>1)]-128;
	v_c = V[VRowstride * (y>>1) + VPixelstride * (x>>1)]-128;

	int r, g, b;
	r = (298 * (y_c) + 409 * (v_c) + 128)>>8;
    g = (298 * (y_c) - 100 * (u_c) - 208 * (v_c) + 128)>>8;
    b = (298 * (y_c) + 516 * (u_c) + 128)>>8;
	
	//saturation	
	r = (r>255)?255:r;
	g = (g>255)?255:g;
	b = (b>255)?255:b;
	  
    r = r<0?0:r; 
    g = g<0?0:g; 
    b = b<0?0:b;

    dst[y_tgt * dst_w + x_tgt] = (uchar4)(
        r,
        g,
        b,
        0	//a
		);
}

__kernel void rgb_planar_from_yuv420_888_crop(
    const uint YRowstride, const uint YPixelstride,
	const uint URowstride, const uint UPixelstride,
	const uint VRowstride, const uint VPixelstride,
	const uint crop_x1, const uint crop_y1, const int dst_w, 
	__global uchar* Y, __global uchar* U, __global uchar* V,
	__global uchar* R, __global uchar* G, __global uchar* B)
{
    const int x_tgt = get_global_id(0);
	const int y_tgt = get_global_id(1);
	const int x = x_tgt + crop_x1;
	const int y = y_tgt + crop_y1;
	int y_c, u_c, v_c;
		
	y_c = Y[YRowstride * y + YPixelstride * x]-16;
	u_c = U[URowstride * (y>>1) + UPixelstride * (x>>1)]-128;
	v_c = V[VRowstride * (y>>1) + VPixelstride * (x>>1)]-128;

	int r, g, b;
	r = (298 * (y_c) + 409 * (v_c) + 128)>>8;
    g = (298 * (y_c) - 100 * (u_c) - 208 * (v_c) + 128)>>8;
    b = (298 * (y_c) + 516 * (u_c) + 128)>>8;
	
	//saturation	
	r = (r>255)?255:r;
	g = (g>255)?255:g;
	b = (b>255)?255:b;
	  
    r = r<0?0:r; 
    g = g<0?0:g; 
    b = b<0?0:b;

	R[y_tgt * dst_w + x_tgt] = r;
	G[y_tgt * dst_w + x_tgt] = g;
	B[y_tgt * dst_w + x_tgt] = b;
    
}

/*
__kernel void rgb8uc4_from_yuv420_888_crop(
    const uint YRowstride, const uint YPixelstride,
	const uint URowstride, const uint UPixelstride,
	const uint VRowstride, const uint VPixelstride,
	const uint crop_x1, const uint crop_y1, const int dst_w, 
	__global uchar* Y, __global uchar* U, __global uchar* V,
	__global uchar3* dst)
{
    const int x_tgt = get_global_id(0);
	const int y_tgt = get_global_id(1);
	const int x = x_tgt + crop_x1;
	const int y = y_tgt + crop_y1;
	int y_c, u_c, v_c;
		
	y_c = Y[YRowstride * y + YPixelstride * x]-16;
	u_c = U[URowstride * (y>>1) + UPixelstride * (x>>1)]-128;
	v_c = V[VRowstride * (y>>1) + VPixelstride * (x>>1)]-128;

	int r, g, b;
	r = (298 * (y_c) + 409 * (v_c) + 128)>>8;
    g = (298 * (y_c) - 100 * (u_c) - 208 * (v_c) + 128)>>8;
    b = (298 * (y_c) + 516 * (u_c) + 128)>>8;
	
	//saturation	
	r = (r>255)?255:r;
	g = (g>255)?255:g;
	b = (b>255)?255:b;
	  
    r = r<0?0:r; 
    g = g<0?0:g; 
    b = b<0?0:b;

    dst[y_tgt * dst_w + x_tgt] = (uchar3)(
        r,
        g,
        b
		);
}
*/