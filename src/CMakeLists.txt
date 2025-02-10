find_package(clock REQUIRED)

find_library(SDL2_LIBRARY
    NAMES SDL2 SDL2-2.0
    HINTS ${SDL2_DIR} /usr/local/lib /usr/lib
)

find_library(SDL2_IMAGE_LIBRARY
    NAMES SDL2_image
    HINTS /usr/local/lib /usr/lib
)
find_library(SDL2_MIXER_LIBRARY
    NAMES SDL2_mixer
    HINTS /usr/local/lib /usr/lib
)

message(STATUS "SDL2_LIBRARY: ${SDL2_LIBRARY}")
message(STATUS "SDL2_IMAGE_LIBRARY: ${SDL2_IMAGE_LIBRARY}")
message(STATUS "SDL2_MIXER_LIBRARY: ${SDL2_MIXER_LIBRARY}")

add_library(engine STATIC
    engine.cpp
    dispatcher.cpp
    graphic_system.cpp
    objects.cpp
    events.cpp
    physics.cpp
    base_object.cpp
)

target_include_directories(engine PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/game_engine>
    $<INSTALL_INTERFACE:include>       # users include as <game_engine/foobar.hpp>
)

if(NOT MINGW)
    target_link_libraries(engine PRIVATE
        box2d                
        SDL2
        SDL2_image
        SDL2_mixer
        SDL2main
    )
endif()

target_link_libraries(engine PRIVATE clock)
