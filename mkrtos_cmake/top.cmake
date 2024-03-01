set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})
set(KCONFIG_ROOT ${CMAKE_SOURCE_DIR}/Kconfig)
set(BOARD_DIR ${CMAKE_SOURCE_DIR}/mkrtos_configs)
set(AUTOCONF_H ${CMAKE_CURRENT_BINARY_DIR}/autoconf.h)

# Re-configure (Re-execute all CMakeLists.txt code) when autoconf.h changes
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${AUTOCONF_H})

include(mkrtos_cmake/extensions.cmake)
include(mkrtos_cmake/python.cmake)
include(mkrtos_cmake/kconfig.cmake)

