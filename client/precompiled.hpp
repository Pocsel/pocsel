#ifndef __CLIENT_PRECOMPILED_HPP__
#define __CLIENT_PRECOMPILED_HPP__

// Détection des leaks mémoire sous VS
#ifdef _MSC_VER
# ifdef DEBUG
#  define _CRTDBG_MAP_ALLOC
#  define _CRTDBG_MAP_ALLOC_NEW
#  include <stdlib.h>
#  include <crtdbg.h>

#  ifndef DEBUG_NEW
#   define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#   define new DEBUG_NEW
#  endif
# endif
#endif

// GLEW - OBLIGATOIREMENT en premier !
#include <GL/glew.h>
#include <SDL/SDL.h>

#ifdef new
# undef new
#endif
#include "tools/precompiled.hpp"
#ifdef DEBUG_NEW
# define new DEBUG_NEW
#endif

// Nos trucs
#include "tools/logger/Logger.hpp"

#endif
