#ifndef __TOOLS_PRECOMPILED_HPP__
#define __TOOLS_PRECOMPILED_HPP__

#ifdef _WIN32
// common/Packet.hpp
#include <Winsock2.h>
#include <WindowsX.h>
// Macros de merde ! (il y en a peut-être d'autres)
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#ifdef LoadString
#undef LoadString
#endif
#ifdef GetCurrentTime
#undef GetCurrentTime
#endif
#ifdef IsMinimized
#undef IsMinimized
#endif
#else
// common/Packet.hpp
#include <arpa/inet.h>
#endif

// Stl
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Boost
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/algorithm/string.hpp> 
#include "tools/filesystem.hpp"

// Others
#include <sqlite3.h>

#include "tools/types.hpp"
#include "tools/Delete.hpp"
#include "tools/ToString.hpp"
#include "tools/sqrt.hpp"

#include "glm/all.hpp"

#ifdef _MSC_VER
// warning this utilisé dans le constructeur
#pragma warning(disable: 4355)
// utilisation de throw() sur un membre
#pragma warning(disable: 4290)
#endif

#ifdef _WIN32
#define THREAD_LOCAL _declspec(thread)
#else
#define THREAD_LOCAL __thread
#endif

#ifdef _MSC_VER
namespace detail {
  template <class T>
  struct type_helper {
    typedef T type;
  };
}

#define decltype(...) detail::type_helper<decltype(__VA_ARGS__)>::type

// CPP11_DELETE utile pour supprimer les constructeurs et opérateurs par copie
#define CPP11_DELETE
#else
#define CPP11_DELETE = delete
#endif

#ifdef _MSC_VER
// dirty hack for MS Compiler
# define _ALLOW_KEYWORD_MACROS
# define noexcept throw()
#endif

#endif
