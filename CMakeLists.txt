cmake_minimum_required(VERSION 3.16)
project(gpuscan CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCL REQUIRED)

add_executable(gpuscan main.cpp)
target_link_libraries(gpuscan PRIVATE OpenCL::OpenCL)
target_compile_definitions(gpuscan PRIVATE
    CL_HPP_TARGET_OPENCL_VERSION=300
    CL_HPP_MINIMUM_OPENCL_VERSION=120
    CL_HPP_ENABLE_EXCEPTIONS)
