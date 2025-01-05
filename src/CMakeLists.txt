find_package(clock)

add_library(engine STATIC
    engine.cpp
    dispatcher.cpp
    graphic_system.cpp
    objects.cpp
)

target_include_directories(engine PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/game_engine>
    $<INSTALL_INTERFACE:include> # users need to include <game_engine/foobar.hpp> instead of <foobar.hpp>
)

target_link_libraries(engine PUBLIC
    SDL2
    SDL2main
    SDL2_image
    SDL2_mixer
)

target_link_libraries(engine PRIVATE $<BUILD_INTERFACE:clock::clock>)

