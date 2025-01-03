if(MINGW)
add_library(clock STATIC "../include/clock_windows.cpp")
else()
add_library(clock STATIC "../include/clock.cpp")
endif()

add_library(engine engine.cpp dispatcher.cpp graphic_system.cpp objects.cpp)
target_include_directories(engine PUBLIC "../include")
target_include_directories(engine PUBLIC .)

target_link_libraries(engine PUBLIC SDL2)
target_link_libraries(engine PUBLIC SDL2main)
target_link_libraries(engine PUBLIC SDL2_image)
target_link_libraries(engine PUBLIC SDL2_mixer)
target_link_libraries(engine PUBLIC clock)

add_executable(main main.cpp)
target_include_directories(main PUBLIC "../include")
target_include_directories(main PUBLIC .)

target_link_libraries(main PUBLIC engine)