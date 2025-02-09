cmake_minimum_required(VERSION 3.20)
project(game_engine VERSION 0.1)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#message("Enabling gprof for Debug build")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg")
#set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -pg")
#set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -pg")

add_subdirectory(src)


#replacement for 
if(MINGW)
option(MINGW_STDTHREADS_GENERATE_STDHEADERS "" ON)
add_subdirectory(mingw-std-threads)
target_link_libraries(engine PRIVATE mingw_stdthreads)
endif()


include(cmake/install_script.cmake)
include(cmake/cpack_script.cmake)

find_package(clock)

add_executable(main main.cpp)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(main PUBLIC engine)
target_link_libraries(main PRIVATE $<BUILD_INTERFACE:clock::clock>)

set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/exec_env
)

