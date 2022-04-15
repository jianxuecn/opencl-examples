/**
 * -------------------------------------------------------------------------------
 * This source file is part of opencl-examples, which is developed for
 * the Cloud Computing Course and the Computer Graphics Course at the School
 * of Engineering Science (SES), University of Chinese Academy of Sciences (UCAS).
 * Copyright (C) 2020-2022 Xue Jian (xuejian@ucas.ac.cn)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * -------------------------------------------------------------------------------
 */
#ifndef __OCLUtils_h
#define __OCLUtils_h

#if defined(__APPLE__)
#include <OpenCL/opencl.h>
#define OCL_FILTERS_SUB_DIR "../Resources/filters"
#else
#include <CL/opencl.h>
#define OCL_FILTERS_SUB_DIR "filters"
#endif

cl_int get_platform_id(cl_platform_id &selectedPlatformID, bool &isNV);
cl_int get_device_id(cl_platform_id platform, cl_device_id &selectedDeviceID, bool &isNV);
cl_device_id get_max_flops_device(cl_context gpuContext);
void list_opencl_devices(cl_platform_id platform);
void show_opencl_device_info(cl_device_id device);
bool load_cl_source_from_string(cl_context gpuContext, char const *src, cl_program &program);
bool load_cl_source_from_file(cl_context gpuContext, char const *filename, cl_program &program);

#endif
