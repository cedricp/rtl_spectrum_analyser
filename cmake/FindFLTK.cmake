## Simple cmake script to find FLTK libraries
## 2016 Cedric PAILLE
##
##

SET(LIBRARY_PATHS
    ${FLTK_ROOT_PATH}/lib
    ${FLTK_ROOT_PATH}/lib64
    /usr/local/lib
    /usr/local/lib64
    /usr/lib
    /usr/lib64
)

SET(INCLUDE_PATHS
    ${FLTK_ROOT_PATH}/include
    /usr/local/include
    /usr/include
)

FIND_PATH( FLTK_INCLUDE_DIRECTORY FL/Fl.H
           PATHS
           ${INCLUDE_PATHS}
           NO_DEFAULT_PATH
           NO_CMAKE_ENVIRONMENT_PATH
           NO_CMAKE_PATH
           NO_SYSTEM_ENVIRONMENT_PATH
           NO_CMAKE_SYSTEM_PATH
           DOC "The directory where Fl.h resides" )
           

IF( NOT DEFINED FLTK_LIB )
  FIND_LIBRARY( FLTK_LIB libfltk.a 
                 PATHS
                 ${LIBRARY_PATHS}
                 NO_DEFAULT_PATH
                 NO_CMAKE_ENVIRONMENT_PATH
                 NO_CMAKE_PATH
                 NO_SYSTEM_ENVIRONMENT_PATH
                 NO_CMAKE_SYSTEM_PATH
                 DOC "The FLTK library" )
ENDIF()
                 
IF( NOT DEFINED FLTK_GL_LIB )
  FIND_LIBRARY( FLTK_GL_LIB libfltk_gl.a 
                 PATHS
                 ${LIBRARY_PATHS}
                 NO_DEFAULT_PATH
                 NO_CMAKE_ENVIRONMENT_PATH
                 NO_CMAKE_PATH
                 NO_SYSTEM_ENVIRONMENT_PATH
                 NO_CMAKE_SYSTEM_PATH
                 DOC "The FLTK-opengl library" )
ENDIF()

IF ( ${FLTK_LIB} STREQUAL "FLTK_SDR_LIB-NOTFOUND" OR ${FLTK_GL_LIB} STREQUAL "FLTK_SDR_LIB-NOTFOUND")
  MESSAGE( FATAL_ERROR "FLTK libraries not found, required (root: ${FLTK_ROOT_PATH})" )
ENDIF()

IF ( ${FLTK_INCLUDE_DIRECTORY} STREQUAL "FLTK_SDR_INCLUDE_DIRECTORY-NOTFOUND" )
  MESSAGE( FATAL_ERROR "FLTK header files not found, required: FLTK_ROOT_PATH: ${FLTK_ROOT_PATH}" )
ENDIF()

SET( FLTK_FOUND TRUE )
SET( FLTK_LIBS ${FLTK_LIB} ${FLTK_GL_LIB} )

MESSAGE( STATUS "FLTK libraries found : ${FLTK_LIBS}")

