Commands to Set Up Environment:

Download VSCode and install via CLI.

sudo apt update
sudo apt install build-essential cake clinfo opencl-headers ocl-icd-opencl-dev

sudo apt install pocl-opencl-icd

sudo apt install g+ opencl-clhpp-headers

sudo apt install intel-opencl-icd

If using other GPU, find and run correct openCl ICD for vendor.

Run using cmake --build build --parallel 2

./build/gpuscan
