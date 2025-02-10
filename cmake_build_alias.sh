cmake -S . -B build -G "Unix Makefiles"  --toolchain cmake/linux_toolchain.cmake -DCMAKE_BUILD_TYPE=Release && cmake --build build
cd build
cpack
mv game_engine-*.deb ..
cd ..
