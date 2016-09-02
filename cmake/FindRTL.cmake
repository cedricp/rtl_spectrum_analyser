## Simple cmake script to find RTL-SDR libraries
## 2016 Cedric PAILLE
##
##

SET(LIBRARY_PATHS
    ${RTL_ROOT_PATH}/lib
    ${RTL_ROOT_PATH}/lib64
    /usr/local/lib
    /usr/local/lib64
    /usr/lib
    /usr/lib64
)

SET(INCLUDE_PATHS
    ${RTL_ROOT_PATH}/include
    /usr/local/include
    /usr/include
)

FIND_PATH( RTL_SDR_INCLUDE_DIRECTORY rtl-sdr.h
           PATHS
           ${INCLUDE_PATHS}
           NO_DEFAULT_PATH
           NO_CMAKE_ENVIRONMENT_PATH
           NO_CMAKE_PATH
           NO_SYSTEM_ENVIRONMENT_PATH
           NO_CMAKE_SYSTEM_PATH
           DOC "The directory where rtl-sdr.h resides" )
           

IF( NOT DEFINED RTL_SDR_LIB )
  FIND_LIBRARY( RTL_SDR_LIB librtlsdr.a
                 PATHS
                 ${LIBRARY_PATHS}
                 NO_DEFAULT_PATH
                 NO_CMAKE_ENVIRONMENT_PATH
                 NO_CMAKE_PATH
                 NO_SYSTEM_ENVIRONMENT_PATH
                 NO_CMAKE_SYSTEM_PATH
                 DOC "The Rtl-Sdr library" )
ENDIF()

IF ( ${RTL_SDR_LIB} STREQUAL "RTL_SDR_LIB-NOTFOUND" )
  MESSAGE( FATAL_ERROR "rtlsdr libraries not found, required (root: ${RTL_ROOT_PATH})" )
ENDIF()

IF ( ${RTL_SDR_INCLUDE_DIRECTORY} STREQUAL "RTL_SDR_INCLUDE_DIRECTORY-NOTFOUND" )
  MESSAGE( FATAL_ERROR "rtl-sdr header files not found, required: RTL_ROOT_PATH: ${RTL_ROOT_PATH}" )
ENDIF()

SET( RTL_SDR_FOUND TRUE )
SET( RTL_SDR_LIBS ${RTL_SDR_LIB} )

MESSAGE( STATUS "Rtl-sdr libraries found : ${RTL_SDR_LIBS}")

