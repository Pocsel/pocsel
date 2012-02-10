#ifndef __TOOLS_LUA_STACKREFPTR_HPP__
#define __TOOLS_LUA_STACKREFPTR_HPP__

namespace Tools { namespace Lua {

    struct StackRef;
    typedef std::shared_ptr<StackRef> StackRefPtr;

}}

#endif
