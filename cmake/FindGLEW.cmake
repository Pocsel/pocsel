#
# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_DIR
# GLEW_LIBRARIES
#

IF (WIN32)
    FIND_PATH( GLEW_INCLUDE_DIR GL/glew.h
        $ENV{PROGRAMFILES}/GLEW/include
        ${GLEW_ROOT_DIR}/include
        ${CMAKE_BINARY_DIR}/glew/include
        DOC "The directory where GL/glew.h resides")

    FIND_LIBRARY( GLEW_LIBRARIES
        NAMES glew GLEW glew32 glew32s
        PATHS
        $ENV{PROGRAMFILES}/GLEW/lib
        ${GLEW_ROOT_DIR}/lib
        ${CMAKE_BINARY_DIR}/glew/lib
        DOC "The GLEW library")
ELSE (WIN32)
    FIND_PATH( GLEW_INCLUDE_DIR GL/glew.h
        /usr/include
        /usr/local/include
        /sw/include
        /opt/local/include
        ${GLEW_ROOT_DIR}/include
        DOC "The directory where GL/glew.h resides")

    # Prefer the static library.
    FIND_LIBRARY( GLEW_LIBRARIES
        NAMES libGLEW.a GLEW
        PATHS
        /usr/lib64
        /usr/lib
        /usr/local/lib64
        /usr/local/lib
        /sw/lib
        /opt/local/lib
        ${GLEW_ROOT_DIR}/lib
        DOC "The GLEW library")
ENDIF (WIN32)

SET(GLEW_FOUND "NO")
IF (GLEW_INCLUDE_DIR AND GLEW_LIBRARIES)
    SET(GLEW_LIBRARIES ${GLEW_LIBRARIES})
    SET(GLEW_FOUND "YES")
ENDIF (GLEW_INCLUDE_DIR AND GLEW_LIBRARIES)

IF (GLEW_FOUND)
    MESSAGE( STATUS "Found GLEW header file in ${GLEW_INCLUDE_DIR}")
    MESSAGE( STATUS "Found GLEW libraries: ${GLEW_LIBRARIES}")
ELSE (GLEW_FOUND)
    IF (GLEW_FIND_REQUIRED)
        MESSAGE( FATAL_ERROR "Could not find GLEW" )
    ELSE (GLEW_FIND_REQUIRED)
        MESSAGE( STATUS "Optional package GLEW was not found" )
    ENDIF (GLEW_FIND_REQUIRED)
ENDIF(GLEW_FOUND)

