sudo apt update
sudo apt install cmake # build tool
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libbox2d-dev # libraries 
rm -rf ./cpp_clock
git clone https://github.com/Al1002/cpp_clock.git # package depends
cd cpp_clock
cmake -S . -B build
cmake --build build
sudo cmake --install build
cd ..
