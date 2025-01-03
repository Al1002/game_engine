cmake_minimum_required(VERSION 3.20)
project(game_engine VERSION 0.1) 


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#message("Enabling gprof for Debug build")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pg")
#set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -pg")
#set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -pg")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/exec_env")

add_subdirectory(src)

#install(TARGETS engine clock
#    EXPORT gameEngineTargets
#    LIBRARY DESTINATION lib
#    ARCHIVE DESTINATION lib
#    # RUNTIME DESTINATION bin
#    INCLUDES DESTINATION include)
#
#install(EXPORT gameEngineTargets
#    FILE gameEngineTargets.cmake
#    NAMESPACE gameEngine::
#    DESTINATION lib/cmake/game_engine)
#
#include(CMakePackageConfigHelpers)
#write_basic_package_version_file(
#    "gameEngineConfigVersion.cmake"
#    VERSION ${PROJECT_VERSION}
#    COMPATIBILITY AnyNewerVersion)
#
#install(FILES "gameEngineConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/gameEngineConfigVersion.cmake"
#    DESTINATION lib/cmake/game_engine)