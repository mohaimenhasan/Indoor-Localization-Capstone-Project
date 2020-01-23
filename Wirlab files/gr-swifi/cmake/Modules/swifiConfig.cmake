INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_SWIFI swifi)

FIND_PATH(
    SWIFI_INCLUDE_DIRS
    NAMES swifi/api.h
    HINTS $ENV{SWIFI_DIR}/include
        ${PC_SWIFI_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    SWIFI_LIBRARIES
    NAMES gnuradio-swifi
    HINTS $ENV{SWIFI_DIR}/lib
        ${PC_SWIFI_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SWIFI DEFAULT_MSG SWIFI_LIBRARIES SWIFI_INCLUDE_DIRS)
MARK_AS_ADVANCED(SWIFI_LIBRARIES SWIFI_INCLUDE_DIRS)

