#ifndef __TOOLS_LUA_EXCEPTION_HPP__
#define __TOOLS_LUA_EXCEPTION_HPP__

#include <stdexcept>

namespace Tools { namespace Lua {

    typedef std::runtime_error ValueError;
    typedef std::runtime_error TypeError;
    typedef std::runtime_error UndefinedError;
    typedef std::runtime_error RuntimeError;

}}

#endif
