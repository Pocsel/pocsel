#
# Try to find GLEW library and include path.
# Once done this will define
#
# CG_FOUND
# CG_INCLUDE_DIR
# CG_LIBRARY
# CGGL_LIBRARY
#
# Windows only:
# CGDX8_LIBRARY
# CGDX9_LIBRARY
# CGDX10_LIBRARY
# CGDX11_LIBRARY
#

if (WIN32)
    FIND_PATH(CG_INCLUDE_DIR Cg/cg.h
        "$ENV{PROGRAMFILES}/NVIDIA Corporation/Cg/include"
        ${CG_ROOT_DIR}/include
        ${CMAKE_BINARY_DIR}/cg/include
        DOC "The directory where Cg/cg.h resides")

    FIND_LIBRARY(CG_LIBRARY NAMES cg
        PATHS
        "$ENV{PROGRAMFILES}/NVIDIA Corporation/Cg/lib"
        ${CG_ROOT_DIR}/lib
        ${CMAKE_BINARY_DIR}/cg/lib
        DOC "The cg library")
    FIND_LIBRARY(CGGL_LIBRARY NAMES cgGL
        PATHS
        "$ENV{PROGRAMFILES}/NVIDIA Corporation/Cg/lib"
        ${CG_ROOT_DIR}/lib
        ${CMAKE_BINARY_DIR}/cg/lib
        DOC "The cg GL library")
    FIND_LIBRARY(CGDX8_LIBRARY NAMES cgD3D8
        PATHS
        "$ENV{PROGRAMFILES}/NVIDIA Corporation/Cg/lib"
        ${CG_ROOT_DIR}/lib
        ${CMAKE_BINARY_DIR}/cg/lib
        DOC "The cg library")
    FIND_LIBRARY(CGDX9_LIBRARY NAMES cgD3D9
        PATHS
        "$ENV{PROGRAMFILES}/NVIDIA Corporation/Cg/lib"
        ${CG_ROOT_DIR}/lib
        ${CMAKE_BINARY_DIR}/cg/lib
        DOC "The cg library")
    FIND_LIBRARY(CGDX10_LIBRARY NAMES cgD3D10
        PATHS
        "$ENV{PROGRAMFILES}/NVIDIA Corporation/Cg/lib"
        ${CG_ROOT_DIR}/lib
        ${CMAKE_BINARY_DIR}/cg/lib
        DOC "The cg library")
    FIND_LIBRARY(CGDX11_LIBRARY NAMES cgD3D11
        PATHS
        "$ENV{PROGRAMFILES}/NVIDIA Corporation/Cg/lib"
        ${CG_ROOT_DIR}/lib
        ${CMAKE_BINARY_DIR}/cg/lib
        DOC "The cg library")

else (WIN32)
    FIND_PATH(CG_INCLUDE_DIR Cg/cg.h
        /usr/include
        /usr/local/include
        /sw/include
        /opt/local/include
        ${CG_ROOT_DIR}/include
        DOC "The directory where Cg/cg.h resides")

    FIND_LIBRARY(CG_LIBRARY
        NAMES libCg.so
        PATHS
        /usr/lib64
        /usr/lib
        /usr/local/lib64
        /usr/local/lib
        /sw/lib
        /opt/local/lib
        ${CG_ROOT_DIR}/lib
        DOC "The cg library")

    FIND_LIBRARY(CGGL_LIBRARY
        NAMES libCgGL.so
        PATHS
        /usr/lib64
        /usr/lib
        /usr/local/lib64
        /usr/local/lib
        /sw/lib
        /opt/local/lib
        ${CG_ROOT_DIR}/lib
        DOC "The cg GL library")
endif (WIN32)

set(CG_FOUND "NO")
if (CG_INCLUDE_DIR AND CG_LIBRARY)
    set(CG_LIBRARIES ${CG_LIBRARY} ${CGGL_LIBRARY} ${CGDX8_LIBRARY}
        ${CGDX9_LIBRARY} ${CGDX10_LIBRARY} ${CGDX11_LIBRARY})
    set(CG_FOUND "YES")
endif ()

if (CG_FOUND)
    message(STATUS "Found Cg header file in ${CG_INCLUDE_DIR}")
    message(STATUS "Found Cg libraries: ${CG_LIBRARIES}")
else ()
    if (CG_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find Cg (set CG_ROOT_DIR)" )
    else ()
        message(STATUS "Optional package Cg was not found" )
    endif ()
endif ()
