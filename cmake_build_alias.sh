echo "Delete build directory"
rm -rf build
mkdir build
cmake -S . -B build -G "Unix Makefiles"  --toolchain cmake/linux_toolchain.cmake -DCMAKE_BUILD_TYPE=Release && cmake --build build
