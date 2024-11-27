add_library(clock "../include/clock.cpp")

add_library(objects objects.cpp)
target_include_directories(objects PUBLIC "../include")
target_include_directories(objects PUBLIC .)

target_link_libraries(objects PUBLIC SDL2)
target_link_libraries(objects PUBLIC SDL2_image)
target_link_libraries(objects PUBLIC clock)

add_executable(main main.cpp)
target_include_directories(main PUBLIC "../include")
target_include_directories(main PUBLIC .)

target_link_libraries(main PUBLIC objects)