__kernel void multiscale_from_yuv420_888(
    const uint YRowstride, const uint YPixelstride,
	const uint URowstride, const uint UPixelstride,
	const uint VRowstride, const uint VPixelstride,
	const uint crop_x1, const uint crop_y1, const uint dst_w,const uint dst_h,
	__global uchar* Y, __global uchar* U, __global uchar* V,
	__global uchar* R, __global uchar* G, __global uchar* B,
	__global uchar* out_Y, __global uchar* out_U, __global uchar* out_V,
	__global uchar* out_Y_resized,
	__global uchar* out_YUV_Y_resized, __global uchar* out_YUV_U_resized, __global uchar* out_YUV_V_resized,
	const uint YUV_resized_w, const uint YUV_resized_h
	)
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

	uint index = y_tgt * dst_w + x_tgt;
	uint uv_index = y_tgt/2 * dst_w/2 + x_tgt/2;
	uint YUV_resized_crop_x = (dst_w/2-YUV_resized_w)/2;
	uint YUV_resized_crop_y = (dst_h/2-YUV_resized_h)/2;

	y_c += 16;
	u_c += 128;
	v_c += 128;

	//yuv planar
	out_Y[index] = y_c;
	if ((x%2==0)&&(y%2==0)){
		out_Y_resized[uv_index] = y_c;
		out_U[uv_index] = u_c;
		out_V[uv_index] = v_c;
		if ((x_tgt/2>=YUV_resized_crop_x) &&
			(x_tgt/2< dst_w/2 - YUV_resized_crop_x) &&
			(y_tgt/2>=YUV_resized_crop_y) &&
			(y_tgt/2< dst_h/2 - YUV_resized_crop_y)){
				uint YUV_resized_index = (y_tgt/2-YUV_resized_crop_y) * YUV_resized_w +(x_tgt/2 - YUV_resized_crop_x);
				out_YUV_Y_resized[YUV_resized_index] = y_c;
				out_YUV_U_resized[YUV_resized_index] = u_c;
				out_YUV_V_resized[YUV_resized_index] = v_c;
			}
	}
	//rgb planar
	R[index] = r;
	G[index] = g;
	B[index] = b;
    
}


__kernel void yv12_from_yuv420_888_crop_resize(
    const uint YRowstride, const uint YPixelstride,
	const uint URowstride, const uint UPixelstride,
	const uint VRowstride, const uint VPixelstride,
	const uint crop_x, const uint crop_y, const uint crop_w, const uint crop_h, 
	__global uchar* Y, __global uchar* U, __global uchar* V,
	__global uchar* dst,
	const int dst_w, const int dst_h)
{ 
	const int x_tgt = get_global_id(0);
	const int y_tgt = get_global_id(1);

	const float gx = x_tgt / (float)dst_w * (crop_w - 1);
    const float gy = y_tgt / (float)dst_h * (crop_h - 1);
	const float tx = gx-(int)gx;
	const float ty = gy-(int)gy;
	const int gxi = (int)gx + crop_x;
	const int gyi = (int)gy + crop_y;
			
	//Y
	int Ybase_index = YRowstride * gyi + YPixelstride * gxi;
	const float y00 = Y[Ybase_index];
	const float y01 = Y[Ybase_index + YRowstride];
	const float y10 = Y[Ybase_index + YPixelstride];
	const float y11 = Y[Ybase_index + YRowstride + YPixelstride];
	dst[dst_w*y_tgt + x_tgt] = (uchar)mix(mix(y00, y10, tx), mix(y01, y11, tx), ty);
	//dst[dst_w*y_tgt + x_tgt] = y00;

	if ((x_tgt%2==0)&&(y_tgt%2==0)){
		//U
		int Ubase_index = URowstride * (gyi >> 1) + UPixelstride * (gxi >> 1);
		const float u00 = U[Ubase_index];
		const float u01 = U[Ubase_index + URowstride * (gyi%2)];
		const float u10 = U[Ubase_index + UPixelstride * (gxi%2)];
		const float u11 = U[Ubase_index + URowstride * (gyi%2) + UPixelstride * (gxi%2)];
		dst[dst_w*dst_h + dst_w/2 * (y_tgt>>1) + (x_tgt>>1)] = (uchar)mix(mix(u00, u10, tx), mix(u01, u11, tx), ty);
		//dst[dst_w*dst_h + dst_w/2 * (y_tgt>>1) + (x_tgt>>1)] = 1;

		//V
		int Vbase_index = VRowstride * (gyi >> 1) + VPixelstride * (gxi >> 1);
		const float v00 = V[Vbase_index];
		const float v01 = V[Vbase_index + VRowstride * (gyi%2)];
		const float v10 = V[Vbase_index + VPixelstride * (gxi%2)];
		const float v11 = V[Vbase_index + VRowstride * (gyi%2) + VPixelstride * (gxi%2)];
		dst[dst_w*dst_h + dst_w/2 *(dst_h/2+ (y_tgt>>1)) + (x_tgt>>1)] = (uchar)mix(mix(v00, v10, tx), mix(v01, v11, tx), ty);
		//dst[dst_w*dst_h + dst_w/2 *(dst_h/2+ (y_tgt>>1)) + (x_tgt>>1)] = 1;
	}
			
}

