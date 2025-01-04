cmake_minimum_required(VERSION 3.20)
project(game_engine VERSION 0.1) 


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#message("Enabling gprof for Debug build")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg")
#set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -pg")
#set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -pg")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")

add_subdirectory(src)

#define 
install(TARGETS engine
    EXPORT gameEngineTargets
    LIBRARY DESTINATION lib
    # ARCHIVE DESTINATION lib
    # RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)

install(EXPORT gameEngineTargets
    FILE gameEngineTargets.cmake
    NAMESPACE gameEngine::
    DESTINATION lib/cmake/game_engine)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "game_engineConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion)

install(FILES "cmake/game_engineConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/game_engineConfigVersion.cmake"
    DESTINATION lib/cmake/game_engine)