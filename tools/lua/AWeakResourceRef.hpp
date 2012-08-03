#ifndef __TOOLS_LUA_AWEAKRESOURCEREF_HPP__
#define __TOOLS_LUA_AWEAKRESOURCEREF_HPP__

namespace Tools { namespace Lua {

    template<typename ManagerType> struct AWeakResourceRef
    {
        virtual ~AWeakResourceRef() {}
        virtual bool IsValid(ManagerType const&) const = 0;
        virtual void Invalidate(ManagerType const&) = 0;
        virtual Ref GetReference(ManagerType const&) const = 0;
        virtual std::string Serialize(ManagerType const&) const { return "return nil"; }
    };

}}

#endif
