find_package(clock REQUIRED)

find_library(SDL2_LIBRARY
    NAMES SDL2 SDL2-2.0
    HINTS ${SDL2_DIR} /usr/local/lib /usr/lib
)

find_library(SDL2_IMAGE_LIBRARY
    NAMES SDL2_image
    HINTS /usr/lib /usr/local/lib
)

find_library(SDL2_MIXER_LIBRARY
    NAMES SDL2_mixer
    HINTS /usr/lib /usr/local/lib
)

add_library(engine STATIC
    engine.cpp
    dispatcher.cpp
    graphic_system.cpp
    objects.cpp
    events.cpp
    physics.cpp
    obj_manager.cpp
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

