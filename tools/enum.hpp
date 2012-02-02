#ifndef __TOOLS_ENUM_HPP__
#define __TOOLS_ENUM_HPP__

#ifdef _WIN32
// Warning pour utilisation d'une extension C++ de VS
#pragma warning(disable: 4482)

# define ENUM enum
#else
# define ENUM enum class
#endif

#endif
