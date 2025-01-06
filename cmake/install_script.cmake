install(TARGETS engine
    EXPORT game_engineTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/game_engine
    DESTINATION include/game_engine
    FILES_MATCHING PATTERN "*.h*"
)

install(EXPORT game_engineTargets
    FILE game_engineTargets.cmake
    NAMESPACE game_engine::
    DESTINATION lib/cmake/game_engine
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    game_engineConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES 
    cmake/game_engineConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/game_engineConfigVersion.cmake
    DESTINATION lib/cmake/game_engine
)