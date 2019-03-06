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

//planar RGB input version
__kernel void luv_from_rgb_planar(
    const float minu, const float minv, const float un, const float vn,
    const float4 mr, const float4 mg, const float4 mb, __constant float* ltable,
    __global float4* dst,
	__global uchar* R, __global uchar* G, __global uchar* B)
{
    const int i = get_global_id(0);
    const float r = R[i];
    const float g = G[i];
    const float b = B[i];
    float x = mr.s0*r + mg.s0*g + mb.s0*b;
    float y = mr.s1*r + mg.s1*g + mb.s1*b;
    float z = mr.s2*r + mg.s2*g + mb.s2*b;
    float l = ltable[(int)(y*1024)];
    z = 1.0f / (x + 15.0f*y + 3.0f*z + 1e-35f);
    dst[i] = (float4)(
        l,
        l*(13.0f*4.0f*x*z - 13.0f*un) - minu,
        l*(13.0f*9.0f*y*z - 13.0f*vn) - minv,
        0.0f);
}

