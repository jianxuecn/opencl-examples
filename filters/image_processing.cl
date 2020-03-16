__kernel void k_smooth_gaussian_uchar4(__global uchar4 *dataOut,
                                       const __global uchar4 *dataIn,
                                       __constant float *gaussKernel,
                                       const int imageWidth,
                                       const int imageHeight,
                                       const int kernelRadius)
{
    const int x = (int)get_global_id(0);
    const int y = (int)get_global_id(1);

    float4 sum = (float4)0;
    const uint ksize = (kernelRadius << 1) + 1;
    for (int j=-kernelRadius; j<=kernelRadius; ++j) {
        const int iy = y+j;
        if (iy<0 || iy>=imageHeight) continue;
        for (int i=-kernelRadius; i<=kernelRadius; ++i) {
            const int ix = x+i;
            if (ix<0 || ix>=imageWidth) continue;
            const uint inidx = iy * imageWidth + ix;
            const uint kidx = (j+kernelRadius) * ksize + (i+kernelRadius);
            const float4 pixelin = convert_float4(dataIn[inidx]);
            sum += pixelin * gaussKernel[kidx];
        }
    }
    
    const uint outidx = y * imageWidth + x;
    const uchar4 final = convert_uchar4(sum);
    //vstore4(final, outidx, dataOut);
    dataOut[outidx] = final;
}

__kernel void k_gradient_uchar4(__global uchar4 *dataOut,
                                const __global uchar4 *dataIn,
                                const int imageWidth,
                                const int imageHeight)
{
    const int x = (int)get_global_id(0);
    const int y = (int)get_global_id(1);
    
    const int x1 = (x+1)%imageWidth;
    const int y1 = (y+1)%imageHeight;
    
    const float4 pixels0 = convert_float4(dataIn[y*imageWidth + x]);
    const float4 pixels1 = convert_float4(dataIn[y*imageWidth + x1]);
    const float4 pixels2 = convert_float4(dataIn[y1*imageWidth + x]);
    const float4 pixels3 = convert_float4(dataIn[y1*imageWidth + x1]);
    
    const float4 gradx = fabs(pixels3 - pixels0);
    const float4 grady = fabs(pixels1 - pixels2);    
    const float4 grad_mag = sqrt(gradx*gradx+grady*grady);
    const uint outidx = y * imageWidth + x;
    const uchar4 final = convert_uchar4(grad_mag);
    final.a = 255;
    //vstore4(final, outidx, dataOut);
    dataOut[outidx] = final;
}

const sampler_t image_sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void k_smooth_gaussian_image(__global uchar4 *dataOut,
                                      __read_only image2d_t imageIn,
                                      __constant float *gaussKernel,
                                      const int imageWidth,
                                      const int imageHeight,
                                      const int kernelRadius)
{
    const int x = (int)get_global_id(0);
    const int y = (int)get_global_id(1);
    
    float4 sum = (float4)0;
    const uint ksize = (kernelRadius << 1) + 1;
    for (int j=-kernelRadius; j<=kernelRadius; ++j) {
        for (int i=-kernelRadius; i<=kernelRadius; ++i) {
            int2 coord = (int2)(x+i, y+j);
            const uint kidx = (j+kernelRadius) * ksize + (i+kernelRadius);
            sum += read_imagef(imageIn, image_sampler, coord) * gaussKernel[kidx];
        }
    }
    
    const uint outidx = y * imageWidth + x;
    const uchar4 final = convert_uchar4(sum*255.0f);
    //vstore4(final.wxyz, outidx, dataOut);
    dataOut[outidx] = final.bgra;
}

__kernel void k_gradient_image(__global uchar4 *dataOut,
                                __read_only image2d_t imageIn,
                                const int imageWidth,
                                const int imageHeight)
{
    const int x = (int)get_global_id(0);
    const int y = (int)get_global_id(1);
    
    const int x1 = (x+1)%imageWidth;
    const int y1 = (y+1)%imageHeight;
    
    const float4 pixels0 = read_imagef(imageIn, image_sampler, (int2)(x, y));
    const float4 pixels1 = read_imagef(imageIn, image_sampler, (int2)(x1, y));
    const float4 pixels2 = read_imagef(imageIn, image_sampler, (int2)(x, y1));
    const float4 pixels3 = read_imagef(imageIn, image_sampler, (int2)(x1, y1));
    
    const float4 gradx = fabs(pixels3 - pixels0);
    const float4 grady = fabs(pixels1 - pixels2);    
    const float4 grad_mag = sqrt(gradx*gradx+grady*grady);
    const uint outidx = y * imageWidth + x;
    const uchar4 final = convert_uchar4(grad_mag*255.0f);
    final.a = 255;
    //vstore4(final.wxyz, outidx, dataOut);
    dataOut[outidx] = final.bgra;
}

// Implementation below is equivalent to linear 2D convolutions for H and V compoonents with:
//      Convo Coefs for Horizontal component
//  {  1,  0, -1,
//     2,  0, -2,
//     1,  0, -1 }
//      Convo Coefs for Vertical component
//  { -1, -2, -1,
//     0,  0,  0,
//     1,  2,  1  };
//*****************************************************************************
__kernel void k_sobel(__global float *dataOut,
                      const __global uchar4 *dataIn,
                      const int imageWidth,
                      const int imageHeight)
{
    const int x = (int)get_global_id(0);
    const int y = (int)get_global_id(1);

    float temp = 0.0f;
    float hSum[3] = { 0.0f, 0.0f, 0.0f };
    float vSum[3] = { 0.0f, 0.0f, 0.0f };

    int pixOffset = (y-1) * imageWidth + x-1;

    // NW
    if (x > 0 && y > 0) {
        hSum[0] += (float)dataIn[pixOffset].x;    // horizontal gradient of Red
        hSum[1] += (float)dataIn[pixOffset].y;    // horizontal gradient of Green
        hSum[2] += (float)dataIn[pixOffset].z;    // horizontal gradient of Blue
        vSum[0] -= (float)dataIn[pixOffset].x;    // vertical gradient of Red
        vSum[1] -= (float)dataIn[pixOffset].y;    // vertical gradient of Green
        vSum[2] -= (float)dataIn[pixOffset].z;    // vertical gradient of Blue   
    }
    pixOffset ++;

    // N
    if (y > 0) {
        vSum[0] -= (float)(dataIn[pixOffset].x << 1);  // vertical gradient of Red
        vSum[1] -= (float)(dataIn[pixOffset].y << 1);  // vertical gradient of Green
        vSum[2] -= (float)(dataIn[pixOffset].z << 1);  // vertical gradient of Blue
    }
    pixOffset ++;

    // NE
    if (y > 0 && x < imageWidth-1) {
        hSum[0] -= (float)dataIn[pixOffset].x;    // horizontal gradient of Red
        hSum[1] -= (float)dataIn[pixOffset].y;    // horizontal gradient of Green
        hSum[2] -= (float)dataIn[pixOffset].z;    // horizontal gradient of Blue
        vSum[0] -= (float)dataIn[pixOffset].x;    // vertical gradient of Red
        vSum[1] -= (float)dataIn[pixOffset].y;    // vertical gradient of Green
        vSum[2] -= (float)dataIn[pixOffset].z;    // vertical gradient of Blue
    }
    pixOffset += imageWidth - 2;

    // W
    if (x > 0) {
        hSum[0] += (float)(dataIn[pixOffset].x << 1);  // vertical gradient of Red
        hSum[1] += (float)(dataIn[pixOffset].y << 1);  // vertical gradient of Green
        hSum[2] += (float)(dataIn[pixOffset].z << 1);  // vertical gradient of Blue
    }
    pixOffset ++;

    // C
    pixOffset ++;

    // E
    if (x < imageWidth-1) {
        hSum[0] -= (float)(dataIn[pixOffset].x << 1);  // vertical gradient of Red
        hSum[1] -= (float)(dataIn[pixOffset].y << 1);  // vertical gradient of Green
        hSum[2] -= (float)(dataIn[pixOffset].z << 1);  // vertical gradient of Blue
    }
    pixOffset += imageWidth - 2;

    // SW
    if (x > 0 && y < imageHeight-1) {
        hSum[0] += (float)dataIn[pixOffset].x;    // horizontal gradient of Red
        hSum[1] += (float)dataIn[pixOffset].y;    // horizontal gradient of Green
        hSum[2] += (float)dataIn[pixOffset].z;    // horizontal gradient of Blue
        vSum[0] += (float)dataIn[pixOffset].x;    // vertical gradient of Red
        vSum[1] += (float)dataIn[pixOffset].y;    // vertical gradient of Green
        vSum[2] += (float)dataIn[pixOffset].z;    // vertical gradient of Blue
    }
    pixOffset ++;

    // S
    if (y < imageHeight-1) {
        vSum[0] += (float)(dataIn[pixOffset].x << 1);  // vertical gradient of Red
        vSum[1] += (float)(dataIn[pixOffset].y << 1);  // vertical gradient of Green
        vSum[2] += (float)(dataIn[pixOffset].z << 1);  // vertical gradient of Blue
    }
    pixOffset ++;

    // SE
    if (x < imageWidth-1 && y < imageHeight-1) {
        hSum[0] -= (float)dataIn[pixOffset].x;    // horizontal gradient of Red
        hSum[1] -= (float)dataIn[pixOffset].y;    // horizontal gradient of Green
        hSum[2] -= (float)dataIn[pixOffset].z;    // horizontal gradient of Blue
        vSum[0] += (float)dataIn[pixOffset].x;    // vertical gradient of Red
        vSum[1] += (float)dataIn[pixOffset].y;    // vertical gradient of Green
        vSum[2] += (float)dataIn[pixOffset].z;    // vertical gradient of Blue
    }

    // Weighted combination of Root-Sum-Square per-color-band H & V gradients for each of RGB
    temp =  0.30f * sqrt((hSum[0] * hSum[0]) + (vSum[0] * vSum[0]));
    temp += 0.55f * sqrt((hSum[1] * hSum[1]) + (vSum[1] * vSum[1]));
    temp += 0.15f * sqrt((hSum[2] * hSum[2]) + (vSum[2] * vSum[2]));

    int outIdx = y * imageWidth + x;
    dataOut[outIdx] = temp;
}

__kernel void k_min_max(__global float *minOut,
                        __global float *maxOut,
                        const __global float *dataIn,
                        const unsigned int n,
                        __local float *minShared,
                        __local float *maxShared)
{
    unsigned int tid = get_local_id(0);
    unsigned int i = get_global_id(0);

    minShared[tid] = (i < n) ? dataIn[i] : FLT_MAX;
    maxShared[tid] = (i < n) ? dataIn[i] : FLT_MIN;

    barrier(CLK_LOCAL_MEM_FENCE);

    // do reduction in shared memory
    for (unsigned int s=get_local_size(0)/2; s>0; s>>=1) {
        if (tid < s) {
            if (minShared[tid+s] < minShared[tid]) minShared[tid] = minShared[tid+s];
            if (maxShared[tid+s] > maxShared[tid]) maxShared[tid] = maxShared[tid+s];            
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (tid == 0) {
        unsigned int outIdx = get_group_id(0);
        minOut[outIdx] = minShared[0];
        maxOut[outIdx] = maxShared[0];
    }
}

__kernel void k_scale_pixels(__global uchar4 *dataOut,
                             const __global float *dataIn,
                             const unsigned int n,
                             const float dataInMin,
                             const float dataInMax)
{
    unsigned int i = get_global_id(0);
    
    if (i < n) {
        float temp = dataIn[i];
        temp = 255.0f * (temp - dataInMin) / (dataInMax - dataInMin);
        dataOut[i] = (uchar4)temp;
        dataOut[i].w = 255;
    }
}

