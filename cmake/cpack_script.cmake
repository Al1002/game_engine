include(InstallRequiredSystemLibraries)

if(LINUX)
    set(CPACK_GENERATOR "DEB")
elseif(WIN32)
    set(CPACK_GENERATOR "NSIS")
else()
    set(CPACK_GENERATOR "ZIP")
endif()

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "Bicagis")
set(CPACK_PACKAGE_CONTACT "aleksandriliev05@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION "Game engine library \n\
    C++ library for game developement."
)
set(CPACK_DEBIAN_PACKAGE_NAME "game-engine")
set(CPACK_DEBIAN_PACKAGE_CONTACT "A. Iliev <aleksandriliev05@gmail.com>")
#set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA ${CMAKE_SOURCE_DIR}/debian/foobar)

include(CPack)