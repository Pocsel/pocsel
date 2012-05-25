#ifndef __COMMON_CUBETYPE_HPP__
#define __COMMON_CUBETYPE_HPP__

#include "common/BaseChunk.hpp"

namespace Common {

    struct CubeType
    {
        BaseChunk::CubeType id;
        std::string name;
        Uint32 pluginId;
        std::string material;

        bool solid;

        CubeType()
        {
        }

        CubeType(BaseChunk::CubeType id, std::string const& name, Uint32 pluginId, std::string const& material, bool solid) :
            id(id),
            name(name),
            pluginId(pluginId),
            material(material),
            solid(solid)
        {
        }
    };

}

#endif
