#ifndef __TOOLS_ARRAY_SIZE_HPP__
#define __TOOLS_ARRAY_SIZE_HPP__

#if 0 // gcc4.5
template<class T, size_t N> constexpr size_t array_size(T (&)[N]){ return N; }
#else
# define array_size(tab) (sizeof(tab) / sizeof(*(tab)))
#endif

#endif
