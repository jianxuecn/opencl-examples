#ifndef __OCLUtils_h
#define __OCLUtils_h

#if defined(__APPLE__)
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

cl_int get_platform_id(cl_platform_id &selectedPlatformID, bool &isNV);
cl_int get_device_id(cl_platform_id platform, cl_device_id &selectedDeviceID, bool &isNV);
cl_device_id get_max_flops_device(cl_context gpuContext);
void list_opencl_devices(cl_platform_id platform);
void show_opencl_device_info(cl_device_id device);
bool load_cl_source_from_string(cl_context gpuContext, char const *src, cl_program &program);
bool load_cl_source_from_file(cl_context gpuContext, char const *filename, cl_program &program);

#endif
