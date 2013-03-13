#ifndef __SERVER_CUBETYPE_HPP__
#define __SERVER_CUBETYPE_HPP__

#include <luasel/Luasel.hpp>

#include "common/CubeType.hpp"

namespace Server { namespace Game { namespace Map {

    struct CubeType : public Common::CubeType
    {
        Tools::Lua::Ref prototype;
        bool transparent;

        CubeType(Common::BaseChunk::CubeType id, std::string const& name, Uint32 pluginId, std::string const& material, bool solid, Tools::Lua::Ref const& prototype, bool transparent) :
            Common::CubeType(id, name, pluginId, material, solid),
            prototype(prototype),
            transparent(transparent)
        {
        }
    };

}}}

#endif
