# opencl-examples

## Introduction

This project provide a set of image processing examples using OpenCL acceleration and Qt GUI for the *Cloud Computing Course* and *Computer Graphics Course* in School of Engineering Science (SES), University of Chinese Academy of Sciences (UCAS).

This projcet demonstrates how to write parallel programs for fast image processing on GPUs via OpenCL, together with a Qt based GUI. Following main topics have been included and implemented:

1. Finding the OpenCL platforms and devices in a computer system;
2. Setting up the OpenCL context and interacting with the OpenCL device through the context;
3. Transfering image data by OpenCL memory objects;
4. Writing the kernel program for some image processing algorithms, including smoothing, gradient field generation and edge detection based on Sobel operator;
5. Running the kernel on the GPU device.

## Dependencies

1. **Qt**: Cross-platform Software Development Framework (https://www.qt.io/) under GNU LGPL license (see https://www.qt.io/download-open-source);
2. **OpenCL**: Open Computing Language, an open standard for cross-platform, parallel programming of diverse accelerators (https://www.khronos.org/opencl/). The dev files for OpenCL are probably provided by the hardware vendor in some kind of dev kit, e.g. CUDA from NVIDIA.

## Instructions for Use

### Prepare develop tools

1. Install Git (optional) to retrieve the source code from GitHub.
2. Install Qt Creator and libraries (Qt 5.15 is preferred) with appropriate C++ compiler (e.g. Clang on macOS, gcc on Linux and MSVC on Windows).
3. Install CMake (https://cmake.org/) for generating native makefiles and workspaces according to the compiler actually used. May be installed together with Qt.
4. Install dev kit for OpenCL according to the type of GPU you want to try the parallel programs on (e.g. CUDA for NVIDIA GPU).

### Setup local workspace

1. Download the source code of this project.

2. Open CMakeLists.txt in the source directory by Qt Creator, then choose appropriate Qt Kit (with compiler) to setup the local project.

### Build the project and have fun!
