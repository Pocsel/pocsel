#ifndef __COMMON_CUBEPOSITION_HPP__
#define __COMMON_CUBEPOSITION_HPP__

#include "tools/Vector3.hpp"
#include "common/BaseChunk.hpp"
#include "common/Position.hpp"

namespace Common {

    struct CubePosition
    {
        BaseChunk::CoordsType world;
        BaseChunk::CoordsType chunk;

        CubePosition() {}
        CubePosition(BaseChunk::CoordsType const& world, Tools::Vector3u const& chk) :
            world(world),
            chunk(chk)
        {
        }
        CubePosition(BaseChunk::CoordsType const& world, Tools::Vector3f const& chk) :
            world(world)
        {
            chunk.x = chk.x;
            chunk.y = chk.y;
            chunk.z = chk.z;


            if (chunk.x >= Common::ChunkSize)
                chunk.x = Common::ChunkSize - 1;
            if (chunk.y >= Common::ChunkSize)
                chunk.y = Common::ChunkSize - 1;
            if (chunk.z >= Common::ChunkSize)
                chunk.z = Common::ChunkSize - 1;
        }
    };

}

#endif
