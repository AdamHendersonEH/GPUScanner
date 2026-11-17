#include <iostream>
#include <CL/cl.h>
#include <vector>

int main()
{
    cl_uint platformCount = 0;

    cl_int result = clGetPlatformIDs(
        0, nullptr, &platformCount
    );

    if (result != CL_SUCCESS || platformCount == 0)
    {
        std::cout << "No OpenCL platforms found.\n";
        return 1;
    }

    std::cout << "OpenCL platforms found: " << platformCount << '\n';

    std::vector<cl_platform_id>platforms(platformCount);
    clGetPlatformIDs(platformCount, platforms.data(), nullptr);
    
    cl_device_id device_id = nullptr;

    for (cl_uint i = 0; i < platformCount; ++i){
        char platformName[256] = {};
        clGetPlatformInfo(
            platforms[i],
            CL_PLATFORM_NAME,
            sizeof(platformName),
            platformName,
            nullptr
        );
        std::cout << "Platform: " << platformName << '\n';
    }

    

    cl_uint deviceCount = 0;

    result = clGetDeviceIDs(
        platforms[0],
        CL_DEVICE_TYPE_GPU,
        0,
        nullptr,
        &deviceCount
    );

    if (result != CL_SUCCESS || deviceCount == 0)
    {
        std::cout << "No OpenCL GPU devices found.\n";
        return 1;
    }

    cl_device_id device;
    clGetDeviceIDs(
        platforms[0],
        CL_DEVICE_TYPE_GPU,
        1,
        &device,
        nullptr
    );

    std::cout << "Device count: " << deviceCount << '\n';
    for (cl_uint i = 0; i < deviceCount; ++i)
    {
        char deviceName[256] = {};
        clGetDeviceInfo(
            device,
            CL_DEVICE_NAME,
            sizeof(deviceName),
            deviceName,
            nullptr
        );
        std::cout << "Device: " << deviceName << '\n';
    }

    cl_device_svm_capabilities svmCapabilities = 0;

    clGetDeviceInfo(
        device,
        CL_DEVICE_SVM_CAPABILITIES,
        sizeof(svmCapabilities),
        &svmCapabilities,
        nullptr
    );

    if (svmCapabilities == 0)
    {
        std::cout << "SVM not supported on this device.\n";
    }
    else
    {
        std::cout << "SVM supported on this device.\n";
    }

    cl_context context = clCreateContext(
        nullptr,
        1,
        &device,
        nullptr,
        nullptr,
        &result
    );

    constexpr size_t N = 5;
    constexpr size_t bufferSize = N * sizeof(int);

    cl_mem buffer = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        bufferSize,
        nullptr,
        &result
    );

     std::vector<int> A(N), rA(N);
    for (int i = 0; i < N; ++i)
    {
        std::cout << "Enter value for A[" << i << "]: ";
        std::cin >> A[i];
    }
    

    cl_command_queue queue = clCreateCommandQueue(
        context,
        device,
        0,
        &result
    );

    clEnqueueWriteBuffer(queue, buffer, CL_TRUE, 0, bufferSize, A.data(), 0, nullptr, nullptr);

    clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, bufferSize, rA.data(), 0, nullptr, nullptr);
    clFinish(queue);

    std::cout << "Buffer contents: ";
    for (int i =0; i < N; ++i){
        std::cout << rA[i] << " ";
    }
    std::cout << std::endl;

    clReleaseMemObject(buffer);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}