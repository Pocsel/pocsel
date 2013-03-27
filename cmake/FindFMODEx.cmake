# - Find FMODEX
# This module finds an installed Fmod package.
#
# It sets the following variables:
#  FMODEX_FOUND       - Set to false, or undefined, if Fmod isn't found.
#  FMODEX_INCLUDE_DIRS - The Fmod include directory.
#  FMODEX_LIBRARY    - The Fmod library to link against.
#
#

FIND_PATH(FMODEX_INCLUDE_DIRS fmod.h)

IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
   FIND_LIBRARY(FMODEX_LIBRARY NAMES fmodex64)
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)
IF(NOT FMODEX_LIBRARY)
   FIND_LIBRARY(FMODEX_LIBRARY NAMES fmodex)
ENDIF(NOT FMODEX_LIBRARY)

IF (FMODEX_INCLUDE_DIRS AND FMODEX_LIBRARY)
   SET(FMODEX_FOUND TRUE)
ENDIF (FMODEX_INCLUDE_DIRS AND FMODEX_LIBRARY)

IF (FMODEX_FOUND)
   # show which Fmod was found only if not quiet
   IF (NOT FMODEX_FIND_QUIETLY)
      MESSAGE(STATUS "Found Fmod: ${FMODEX_LIBRARY}")
   ENDIF (NOT FMODEX_FIND_QUIETLY)
ELSE (FMODEX_FOUND)
   # fatal error if Fmod is required but not found
   IF (FMODEX_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find FMODEX... (aka libfmodex)")
   ENDIF (FMODEX_FIND_REQUIRED)
ENDIF (FMODEX_FOUND)
