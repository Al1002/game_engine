add_executable(main main.cpp)
target_include_directories(main PUBLIC "../include")
target_include_directories(main PUBLIC .)

target_link_libraries(main PRIVATE SDL2)
target_link_libraries(main PRIVATE SDL2_image)
