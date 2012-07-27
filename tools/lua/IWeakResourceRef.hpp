#ifndef __TOOLS_LUA_IWEAKRESOURCEREF_HPP__
#define __TOOLS_LUA_IWEAKRESOURCEREF_HPP__

namespace Tools { namespace Lua {

    template<typename ManagerType> struct IWeakResourceRef
    {
        virtual ~IWeakResourceRef() {}
        virtual bool IsValid(ManagerType const&) const = 0;
        virtual void Invalidate(ManagerType const&) = 0;
        virtual Ref GetReference(ManagerType const&) const = 0;
    };

}}

#endif
