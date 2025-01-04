if(MINGW)
add_library(clock STATIC "../include/clock_windows.cpp")
else()
add_library(clock STATIC "../include/clock.cpp")
endif()

add_library(engine STATIC
    engine.cpp
    dispatcher.cpp
    graphic_system.cpp
    objects.cpp
)

target_include_directories(engine PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:include/game_engine>
)

target_link_libraries(engine PUBLIC
    SDL2
    SDL2main
    SDL2_image
    SDL2_mixer
)

target_link_libraries(engine PRIVATE $<BUILD_INTERFACE:clock>)

add_executable(main main.cpp)
target_include_directories(main PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(main PUBLIC engine)