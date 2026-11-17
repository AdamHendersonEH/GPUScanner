#include <iostream>
#include <CL/cl.h>
#include <vector>
#include <yara.h>

int yaraCallback(YR_SCAN_CONTEXT* context, int message,
                 void* messageData, void* userData)
{
    if (message == CALLBACK_MSG_RULE_MATCHING)
    {
        YR_RULE* rule = static_cast<YR_RULE*>(messageData);
        std::cout << "YARA match: " << rule->identifier << '\n';
        *static_cast<int*>(userData) += 1;
    }
    return CALLBACK_CONTINUE;
}

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

    const char* detect_1234 = R"(
    rule contains_1234
    {
    strings:
        $a = { D2 04 00 00 }
    condition:
        $a
    }
    )";

    if (yr_initialize() != ERROR_SUCCESS)
    {
        std::cerr << "Failed to initialize YARA." << std::endl;
        return 1;
    }

    YR_COMPILER* compiler = nullptr;
    YR_RULES* rules = nullptr;

    if (yr_compiler_create(&compiler) != ERROR_SUCCESS)
    {
        std::cerr << "Failed to create YARA compiler." << std::endl;
        yr_finalize();
        return 1;
    }

    if (yr_compiler_add_string(compiler, detect_1234, nullptr) != ERROR_SUCCESS)
    {
        std::cerr << "Failed to add string to YARA compiler." << std::endl;
        yr_compiler_destroy(compiler);
        yr_finalize();
        return 1;
    }

    yr_compiler_get_rules(compiler, &rules);

    int matches = 0;
    int scan_result = yr_rules_scan_mem(rules, reinterpret_cast<const uint8_t*>(rA.data()), bufferSize, 0, yaraCallback, &matches, 1000);

    if (scan_result == ERROR_SUCCESS)
    {
        std::cout << "YARA scan completed successfully." << std::endl;
    }
    else
    {
        std::cerr << "YARA scan failed with error code: " << scan_result << std::endl;
    }
    yr_rules_destroy(rules);
    yr_compiler_destroy(compiler);
    yr_finalize();

    clReleaseMemObject(buffer);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}