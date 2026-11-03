#include <iostream>
#include <CL/cl.h>

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

    std::cout << "OpenCL platforms found: "
              << platformCount << '\n';

    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, nullptr);

    char platformName[256] = {};

    clGetPlatformInfo(
        platform,
        CL_PLATFORM_NAME,
        sizeof(platformName),
        platformName,
        nullptr
    );

    std::cout << "Platform: " << platformName << '\n';

    cl_uint deviceCount = 0;

    result = clGetDeviceIDs(
        platform,
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
        platform,
        CL_DEVICE_TYPE_GPU,
        1,
        &device,
        nullptr
    );

    char deviceName[256] = {};

    clGetDeviceInfo(
        device,
        CL_DEVICE_NAME,
        sizeof(deviceName),
        deviceName,
        nullptr
    );

    std::cout << "GPU: " << deviceName << '\n';

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

    cl_mem buffer = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        10 * sizeof(int),
        nullptr,
        &result
    );

    int A[] = {0,0,0,0,0,0,0,0,0,0};
    for (int i= 0; i < 10; ++i)
    {
        std::cout << "Enter value for A[" << i << "]: ";
        std::cin >> A[i];
    }
    

    cl_command_queue queue = clCreateCommandQueueWithProperties(
        context,
        device,
        0,
        &result
    );

    clEnqueueWriteBuffer(queue, buffer, CL_TRUE, 0, sizeof(int) * 10, A, 0, nullptr, nullptr);

    int rA[10];
    clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, sizeof(int) * 10, rA, 0, nullptr, nullptr);
    clFinish(queue);

    std::cout << "Buffer contents: ";
    for (int i =0; i < 10; ++i){
        std::cout << rA[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}