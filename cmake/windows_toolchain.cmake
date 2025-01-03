set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)

set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_AR x86_64-w64-mingw32-ar)
set(CMAKE_RANLIB x86_64-w64-mingw32-ranlib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
-static-libgcc -static-libstdc++ \
-I/usr/local/x86_64-w64-mingw32/include -L/usr/local/x86_64-w64-mingw32/lib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-static-libgcc -static-libstdc++ -I/usr/local/x86_64-w64-mingw32/include \
-L/usr/local/x86_64-w64-mingw32/lib")
