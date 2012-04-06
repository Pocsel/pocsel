#ifndef __SERVER_CUBETYPE_HPP__
#define __SERVER_CUBETYPE_HPP__

#include "common/CubeType.hpp"

#include "tools/lua/Ref.hpp"

namespace Server { namespace Game { namespace Map {

    struct CubeType : public Common::CubeType
    {
        Tools::Lua::Ref dataLua;

        CubeType(Common::BaseChunk::CubeType id, std::string const& name, Tools::Lua::Ref const& data)
            : Common::CubeType(id, name),
            dataLua(data)
        {
        }
    };

}}}

#endif
