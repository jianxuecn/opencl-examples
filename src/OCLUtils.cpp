#include "OCLUtils.h"
#include "LogManager.h"

cl_int get_platform_id(cl_platform_id &selectedPlatformID, bool &isNV)
{
    char chBuffer[1024];
    cl_uint platformCount; 
    cl_platform_id *platformIDs = NULL;
    cl_int errNum;
    selectedPlatformID = NULL;
    isNV = false;

    // Get OpenCL platform count
    errNum = clGetPlatformIDs(0, NULL, &platformCount);
    if (errNum != CL_SUCCESS) {
        ERROR_MESSAGE("clGetPlatformIDs call error: " << errNum);
        return errNum;
    } else {
        if (platformCount == 0) {
            ERROR_MESSAGE("No OpenCL platform found!");
            return -2000;
        } else {
            // if there's a platform or more, make space for ID's
            if ((platformIDs = new cl_platform_id[platformCount]) == NULL) {
                ERROR_MESSAGE("Failed to allocate memory for cl_platform ID's!");
                return -3000;
            }

            // get platform info for each platform and trap the NVIDIA platform if found
            errNum = clGetPlatformIDs(platformCount, platformIDs, NULL);
            cl_uint nvIdx = platformCount;
            cl_uint amdIdx = platformCount;
            cl_uint appleIdx = platformCount;
            for (cl_uint i = 0; i < platformCount; ++i) {
                errNum = clGetPlatformInfo(platformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
                if (errNum == CL_SUCCESS) {
                    LOG_INFO("Detected platform " << i+1 << ": " << chBuffer);
                    if(strstr(chBuffer, "NVIDIA") != NULL) {
                        //selectedPlatformID = platformIDs[i];
                        //LOG_INFO("Selected platform: " << chBuffer);
                        nvIdx = i;
                        //break;
                    } else if (strstr(chBuffer, "AMD") != NULL) {
                        amdIdx = i;
                        //break;
                    } else if (strstr(chBuffer, "Apple") != NULL) {
                        appleIdx = i;
                    }
                }
            }

            if (nvIdx < platformCount) {
                selectedPlatformID = platformIDs[nvIdx];
                isNV = true;
            } else if (amdIdx < platformCount) {
                selectedPlatformID = platformIDs[amdIdx];
            } else if (appleIdx < platformCount) {
                selectedPlatformID = platformIDs[appleIdx];
            }

            // default to zeroeth platform if NVIDIA or AMD not found
            if (selectedPlatformID == NULL) {
                WARNING_MESSAGE("NVIDIA or AMD OpenCL platform not found - defaulting to first platform!");
                selectedPlatformID = platformIDs[0];
            }

			errNum = clGetPlatformInfo(selectedPlatformID, CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
			if (errNum == CL_SUCCESS) LOG_INFO("Name of the selected platform: " << chBuffer);

			errNum = clGetPlatformInfo(selectedPlatformID, CL_PLATFORM_VENDOR, 1024, &chBuffer, NULL);
			if (errNum == CL_SUCCESS) LOG_INFO("Vendor of the selected platform: " << chBuffer);

			errNum = clGetPlatformInfo(selectedPlatformID, CL_PLATFORM_VERSION, 1024, &chBuffer, NULL);
			if (errNum == CL_SUCCESS) LOG_INFO("Version of the selected platform: " << chBuffer);

			errNum = clGetPlatformInfo(selectedPlatformID, CL_PLATFORM_PROFILE, 1024, &chBuffer, NULL);
			if (errNum == CL_SUCCESS) LOG_INFO("Profile of the selected platform: " << chBuffer);

			errNum = clGetPlatformInfo(selectedPlatformID, CL_PLATFORM_EXTENSIONS, 1024, &chBuffer, NULL);
			if (errNum == CL_SUCCESS) LOG_INFO("Extensions of the selected platform: " << chBuffer);

            delete []platformIDs;
        }
    }

    return CL_SUCCESS;
}

cl_int get_device_id(cl_platform_id platform, cl_device_id &selectedDeviceID, bool &isNV)
{
    char chBuffer[1024];
    cl_uint deviceCount;
    cl_device_id *deviceIDs = NULL;
    cl_int errNum;
    selectedDeviceID = NULL;
    isNV = false;
    
    errNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &deviceCount);
    if (errNum != CL_SUCCESS) {
        ERROR_MESSAGE("clGetDeviceIDs call error: " << errNum);
        return errNum;
    }
    
    if (deviceCount == 0) {
        ERROR_MESSAGE("No OpenCL device found!");
        return -2000;
    }
    
    // if there's a platform or more, make space for ID's
    if ((deviceIDs = new cl_device_id[deviceCount]) == NULL) {
        ERROR_MESSAGE("Failed to allocate memory for cl_device ID's!");
        return -3000;
    }
    
    errNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, deviceCount, deviceIDs, NULL);
    cl_uint nvIdx = deviceCount;
    cl_uint amdIdx = deviceCount;
    for (cl_uint i = 0; i < deviceCount; ++i) {
        errNum = clGetDeviceInfo(deviceIDs[i], CL_DEVICE_VENDOR, sizeof(chBuffer), &chBuffer, NULL);
        if (errNum == CL_SUCCESS) {
            LOG_INFO("Detected device " << i+1 << ": " << chBuffer);
            if(strstr(chBuffer, "NVIDIA") != NULL) {
                //selectedPlatformID = platformIDs[i];
                //LOG_INFO("Selected platform: " << chBuffer);
                nvIdx = i;
                break;
            }
            if (strstr(chBuffer, "AMD") != NULL) {
                amdIdx = i;
                //break;
            }
        }
    }
    
    if (nvIdx < deviceCount) {
        selectedDeviceID = deviceIDs[nvIdx];
        isNV = true;
    } else if (amdIdx < deviceCount) {
        selectedDeviceID = deviceIDs[amdIdx];
    }
    
    // default to zeroeth platform if NVIDIA or AMD not found
    if (selectedDeviceID == NULL) {
        WARNING_MESSAGE("NVIDIA or AMD OpenCL device not found - defaulting to first device!");
        selectedDeviceID = deviceIDs[0];
    }
    
    delete []deviceIDs;

    return CL_SUCCESS;
}

cl_device_id get_max_flops_device(cl_context gpuContext)
{
    size_t parmDataBytes;
    cl_device_id* devices;

    // get the list of GPU devices associated with context
    clGetContextInfo(gpuContext, CL_CONTEXT_DEVICES, 0, NULL, &parmDataBytes);
    size_t deviceCount = parmDataBytes / sizeof(cl_device_id);
    devices = new cl_device_id[deviceCount];
    
    LOG_INFO("Number of devices: " << deviceCount);

    clGetContextInfo(gpuContext, CL_CONTEXT_DEVICES, parmDataBytes, devices, NULL);

    cl_device_id maxFlopsDevice = devices[0];
    cl_uint maxFlops = 0;

    size_t currentDevice = 0;

    // CL_DEVICE_MAX_COMPUTE_UNITS
    cl_uint computeUnits;
    clGetDeviceInfo(devices[currentDevice], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUnits), &computeUnits, NULL);

    // CL_DEVICE_MAX_CLOCK_FREQUENCY
    cl_uint clockFrequency;
    clGetDeviceInfo(devices[currentDevice], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clockFrequency), &clockFrequency, NULL);

    LOG_INFO("Device " << currentDevice << ": Number of Compute Units = " << computeUnits << ", Clock Frequency = " << clockFrequency);

    maxFlops = computeUnits * clockFrequency;
    ++currentDevice;

    while( currentDevice < deviceCount ) {
        // CL_DEVICE_MAX_COMPUTE_UNITS
        //cl_uint computeUnits;
        clGetDeviceInfo(devices[currentDevice], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUnits), &computeUnits, NULL);

        // CL_DEVICE_MAX_CLOCK_FREQUENCY
        //cl_uint clockFrequency;
        clGetDeviceInfo(devices[currentDevice], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clockFrequency), &clockFrequency, NULL);

        LOG_INFO("Device " << currentDevice << ": Number of Compute Units = " << computeUnits << ", Clock Frequency = " << clockFrequency);

        cl_uint flops = computeUnits * clockFrequency;
        if( flops > maxFlops ) {
            maxFlops = flops;
            maxFlopsDevice = devices[currentDevice];
        }
        //maxFlopsDevice = devices[currentDevice];
        ++currentDevice;
    }

    delete []devices;


    return maxFlopsDevice;
}

// Beginning of GPU Architecture definitions
// NVIDIA only
inline int convert_sm_ver_to_cores(int major, int minor)
{
    // Defines for GPU Architecture types (using the SM version to determine the # of cores per SM
    typedef struct {
        int SM; // 0xMm (hexidecimal notation), M = SM Major version, and m = SM minor version
        int Cores;
    } sSMtoCores;

    sSMtoCores nGpuArchCoresPerSM[] = {
        { 0x10,   8 }, // Tesla Generation (SM 1.0) G80 class
        { 0x11,   8 }, // Tesla Generation (SM 1.1) G8x class
        { 0x12,   8 }, // Tesla Generation (SM 1.2) G9x class
        { 0x13,   8 }, // Tesla Generation (SM 1.3) GT200 class
        { 0x20,  32 }, // Fermi Generation (SM 2.0) GF100 class
        { 0x21,  48 }, // Fermi Generation (SM 2.1) GF10x class
        { 0x30, 192 }, // Fermi Generation (SM 3.0) GK10x class
        { 0x32, 192 }, // Kepler Generation (SM 3.2) GK10x class
        { 0x35, 192 }, // Kepler Generation (SM 3.5) GK11x class
        { 0x37, 192 }, // Kepler Generation (SM 3.7) GK21x class
        { 0x50, 128 }, // Maxwell Generation (SM 5.0) GM10x class
        { 0x52, 128 }, // Maxwell Generation (SM 5.2) GM20x class 
        { 0x53, 128 }, // Maxwell Generation (SM 5.3) GM20x class
        { 0x60,  64 }, // Pascal Generation (SM 6.0) GP100 class
        { 0x61, 128 }, // Pascal Generation (SM 6.1) GP10x class
        { 0x62, 128 }, // Pascal Generation (SM 6.2) GP10x class
        { 0x70,  64 }, // Volta Generation (SM 7.0) GV100 class
        { 0x72,  64 }, // Volta Generation (SM 7.2) GV11b class
        { 0x75,  64 },
        {   -1,  -1 }
    };

    int index = 0;
    while (nGpuArchCoresPerSM[index].SM != -1) {
        if (nGpuArchCoresPerSM[index].SM == ((major << 4) + minor) ) {
            return nGpuArchCoresPerSM[index].Cores;
        }
        index++;
    }
    LOG_INFO("MapSMtoCores SM " << major << "." << minor << " is undefined (please update to the latest SDK)!");
    return -1;
}
// end of GPU Architecture definitions

void list_opencl_devices(cl_platform_id platform)
{
    char chBuffer[1024];
    cl_uint deviceCount;
    cl_device_id *deviceIDs = NULL;
    cl_int errNum;
    
    errNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &deviceCount);
    if (errNum != CL_SUCCESS) {
        ERROR_MESSAGE("clGetDeviceIDs call error: " << errNum);
        return;
    }
    
    if (deviceCount == 0) {
        ERROR_MESSAGE("No OpenCL device found!");
        return;
    }
    
    // if there's a platform or more, make space for ID's
    if ((deviceIDs = new cl_device_id[deviceCount]) == NULL) {
        ERROR_MESSAGE("Failed to allocate memory for cl_device ID's!");
        return;
    }
    
    errNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, deviceCount, deviceIDs, NULL);
    for (cl_uint i = 0; i < deviceCount; ++i) {
        errNum = clGetDeviceInfo(deviceIDs[i], CL_DEVICE_NAME, sizeof(chBuffer), &chBuffer, NULL);
        if (errNum == CL_SUCCESS) {
            LOG_INFO("Detected device " << i+1 << ": " << chBuffer);
        }
    }
}

void show_opencl_device_info(cl_device_id device)
{
    char deviceString[1024];
    bool nvDevice = false;

    // CL_DEVICE_NAME
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(deviceString), &deviceString, NULL);
    LOG_INFO("OpenCL device name: " << deviceString);

    clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(deviceString), &deviceString, NULL);
    LOG_INFO("OpenCL device vendor: " << deviceString);
    if(strstr(deviceString, "NVIDIA") != NULL) nvDevice = true;

    cl_uint maxComputeUnits;
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, NULL);
    LOG_INFO("Max compute units: " << maxComputeUnits);

    // NVIDIA only
#if defined(CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV)
    if (nvDevice) {
        cl_uint computeCapabilityMajor, computeCapabilityMinor;
        clGetDeviceInfo(device, CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV, sizeof(cl_uint), &computeCapabilityMajor, NULL);
        clGetDeviceInfo(device, CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV, sizeof(cl_uint), &computeCapabilityMinor, NULL);
        LOG_INFO("Device compute capability (NV): " << computeCapabilityMajor << "." << computeCapabilityMinor);
        LOG_INFO("Number of CUDA cores (NV): " << convert_sm_ver_to_cores(computeCapabilityMajor, computeCapabilityMinor) * maxComputeUnits);

        cl_bool gpuOverlap;
        clGetDeviceInfo(device, CL_DEVICE_GPU_OVERLAP_NV, sizeof(cl_bool), &gpuOverlap, NULL);
        LOG_INFO("Device support GPU overlap (NV): " << (gpuOverlap ? "TRUE" : "FALSE"));
    }
#endif

    cl_uint maxClockFrequency;
    clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(maxClockFrequency), &maxClockFrequency, NULL);
    LOG_INFO("Max clock frequency: " << maxClockFrequency << "MHz");

    cl_uint num;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(num), &num, NULL);
    LOG_INFO("Max work item dimensions: " << num);
    size_t workItemSizes[3];
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t)*num, workItemSizes, NULL);
    std::ostringstream msg;
    for (cl_uint i=0; i<num; ++i) msg << workItemSizes[i] << " ";
    LOG_INFO("Max work item sizes: " << msg.str().c_str());
    //LOG_INFO("Max work item sizes: " << workItemSizes[0] << " " << workItemSizes[1] << " " << workItemSizes[2]);

    size_t workgroup_size;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);
    LOG_INFO("Max work group size: " << workgroup_size);

    size_t wMax, hMax, dMax;
    clGetDeviceInfo(device, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(wMax), &wMax, NULL);
    clGetDeviceInfo(device, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(hMax), &hMax, NULL);
    LOG_INFO("Max size of 2D image: " << wMax << " x " << hMax);
    clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(wMax), &wMax, NULL);
    clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(hMax), &hMax, NULL);
    clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(dMax), &dMax, NULL);
    LOG_INFO("Max size of 3D image: " << wMax << " x " << hMax << " x " << dMax);

    cl_ulong maxMemAllocSize;
    clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(maxMemAllocSize), &maxMemAllocSize, NULL);
    LOG_INFO("Max memory allocate size: " << maxMemAllocSize/(1024.0*1024.0) << "MB");

    cl_ulong memSize;
    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(memSize), &memSize, NULL);
    LOG_INFO("Global memory size: " << memSize/(1024.0*1024.0) << "MB");

    cl_bool imageSupport;
    clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(imageSupport), &imageSupport, NULL);
    LOG_INFO("Device support image: " << (imageSupport ? "TRUE" : "FALSE"));

    clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, sizeof(deviceString), &deviceString, NULL);
    LOG_INFO("Device extensions: " << deviceString);
}

bool load_cl_source_from_file(cl_context gpuContext, char const *filename, cl_program &program)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        ERROR_MESSAGE("Cannot open file " << filename);
        return false;
    }

    fseek(fp, 0, SEEK_END);
    size_t len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *src = new char[len+1];
    if (!src) {
        ERROR_MESSAGE("Cannot allocate memory for OpenCL source code!");
        return false;
    }

    bool res = true;
    if (fread(src, sizeof(char), len, fp)!=len) {
        ERROR_MESSAGE("Cannot load source code!");
        res = false;
    } else {
        src[len] = 0;
        cl_int retCode;
        program = clCreateProgramWithSource(gpuContext, 1, (const char **)&src, &len, &retCode);
        if (retCode != CL_SUCCESS) {
            ERROR_MESSAGE("clCreateProgramWithSource call error: " << retCode);
            res = false;
        }
    }

    delete []src;
    return res;
}

bool load_cl_source_from_string( cl_context gpuContext, char const *src, cl_program &program )
{
    if (!src) return false;
    cl_int retCode;
    size_t len = strlen(src);
    program = clCreateProgramWithSource(gpuContext, 1, (const char **)&src, &len, &retCode);
    if (retCode != CL_SUCCESS) {
        ERROR_MESSAGE("clCreateProgramWithSource call error: " << retCode);
        return false;
    }
    return true;
}

