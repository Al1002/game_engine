echo "Delete build_win directory"
rm -rf build_win
mkdir build_win
cmake -S . -B build_win -G "Unix Makefiles" build_win --toolchain cmake/windows_toolchain.cmake -DCMAKE_BUILD_TYPE=Release && cmake --build build_win
