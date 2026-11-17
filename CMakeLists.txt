cmake_minimum_required(VERSION 3.16)
project(gpuscan CXX)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCL REQUIRED)
find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(YARA QUIET yara)
endif()

if(NOT YARA_FOUND)
    find_path(YARA_INCLUDE_DIRS yara.h
        HINTS ENV YARA_ROOT
        PATH_SUFFIXES include)
    find_library(YARA_LIBRARIES NAMES yara libyara
        HINTS ENV YARA_ROOT
        PATH_SUFFIXES lib)
    if(NOT YARA_INCLUDE_DIRS OR NOT YARA_LIBRARIES)
        message(FATAL_ERROR "YARA not found. Install it or set -DYARA_ROOT / -DCMAKE_PREFIX_PATH.")
    endif()
endif()


add_executable(gpuscan main.cpp)
target_include_directories(gpuscan PRIVATE ${YARA_INCLUDE_DIRS})
target_link_directories(gpuscan PRIVATE ${YARA_LIBRARY_DIRS})
target_link_libraries(gpuscan PRIVATE OpenCL::OpenCL ${YARA_LIBRARIES})
target_compile_definitions(gpuscan PRIVATE
    CL_HPP_TARGET_OPENCL_VERSION=120
    CL_HPP_MINIMUM_OPENCL_VERSION=120
    CL_HPP_ENABLE_EXCEPTIONS)
