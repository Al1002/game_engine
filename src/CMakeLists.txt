add_library(clock "../include/clock.cpp")

add_library(engine engine.cpp dispatcher.cpp graphic_system.cpp objects.cpp)
target_include_directories(engine PUBLIC "../include")
target_include_directories(engine PUBLIC .)

target_link_libraries(engine PUBLIC SDL2)
target_link_libraries(engine PUBLIC SDL2main)
target_link_libraries(engine PUBLIC SDL2_image)
target_link_libraries(engine PUBLIC SDL2_mixer)
target_link_libraries(engine PRIVATE clock)

add_executable(main main.cpp)
target_include_directories(main PUBLIC "../include")
target_include_directories(main PUBLIC .)

target_link_libraries(main PUBLIC engine)