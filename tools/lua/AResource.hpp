#ifndef __TOOLS_LUA_ARESOURCE_HPP__
#define __TOOLS_LUA_ARESOURCE_HPP__

namespace Tools { namespace Lua {

    template<typename ResourceManager> struct AResource
    {
        virtual ~AResource()
        {
        }

        virtual void Index(ResourceManager&, CallHelper&)
        {
            throw std::runtime_error("Tools::Lua::AResource: Metamethod __index is not implemented for this type of resource");
        }

        virtual void NewIndex(ResourceManager&, CallHelper&)
        {
            throw std::runtime_error("Tools::Lua::AResource: Metamethod __newindex is not implemented for this type of resource");
        }

        virtual bool IsValid() = 0;
        virtual void Invalidate() = 0;
    };

}}

#endif
