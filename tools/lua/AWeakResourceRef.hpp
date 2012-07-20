#ifndef __TOOLS_LUA_AWEAKRESOURCEREF_HPP__
#define __TOOLS_LUA_AWEAKRESOURCEREF_HPP__

namespace Tools { namespace Lua {

    template<typename ManagerType> struct AWeakResourceRef
    {
        virtual ~WeakResourceRef()
        {
        }

        virtual void Lock(ManagerType&, CallHelper&) = 0;
    };

}}

#endif
