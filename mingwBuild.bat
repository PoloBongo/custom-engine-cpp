if not exist build mkdir build
cd build
cmake -S ../ -B . -G "MinGW makefiles"
mingw32-make.exe && mingw32-make.exe Shaders
cd ..