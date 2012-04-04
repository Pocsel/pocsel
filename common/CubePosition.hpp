#ifndef __COMMON_CUBEPOSITION_HPP__
#define __COMMON_CUBEPOSITION_HPP__

#include "tools/Vector3.hpp"
#include "common/BaseChunk.hpp"

namespace Common {

    typedef Tools::Vector3u CubePosition;

    /* coordonnées entières du chunk contenant ce cube */
    inline BaseChunk::CoordsType GetChunkCoords(CubePosition const& pos);

    /* coordonnées entières du cube relatives au chunk le contenant */
    inline BaseChunk::CoordsType GetCubeCoordsInChunk(CubePosition const& pos);

}

namespace Common {

    BaseChunk::CoordsType GetChunkCoords(CubePosition const& pos)
    {
        return pos / Common::ChunkSize;
    }

    BaseChunk::CoordsType GetCubeCoordsInChunk(CubePosition const& pos)
    {
        return pos - GetChunkCoords(pos) * Common::ChunkSize;
    }

}

//namespace Common {
//
//    struct CubePosition
//    {
//        BaseChunk::CoordsType world;
//        BaseChunk::CoordsType chunk;
//
//        CubePosition() {}
//        //CubePosition(Tools::Vector3d /* Position */ const& pos) :
//        //    world(GetChunkCoords(pos)),
//        //    chunk(GetCubeCoordsInChunk(pos))
//        //{
//        //}
//        CubePosition(BaseChunk::CoordsType const& world, Tools::Vector3u const& chk) :
//            world(world),
//            chunk(chk)
//        {
//        }
//        CubePosition(BaseChunk::CoordsType const& world, Tools::Vector3f const& chk) :
//            world(world)
//        {
//            chunk = BaseChunk::CoordsType(chk);
//
//            if (chunk.x >= Common::ChunkSize)
//                chunk.x = Common::ChunkSize - 1;
//            if (chunk.y >= Common::ChunkSize)
//                chunk.y = Common::ChunkSize - 1;
//            if (chunk.z >= Common::ChunkSize)
//                chunk.z = Common::ChunkSize - 1;
//        }
//    };
//
//}

#endif
