#ifndef __COMMON_CUBETYPE_HPP__
#define __COMMON_CUBETYPE_HPP__

#include "common/BaseChunk.hpp"

namespace Common {

    struct CubeType
    {
        BaseChunk::CubeType id;
        std::string name;
        Uint32 visualEffect;

        bool solid;
        bool transparent;

        CubeType()
        {
        }

        CubeType(BaseChunk::CubeType id, std::string const& name) :
            id(id),
            name(name)
        {
        }
    };

}

#endif
