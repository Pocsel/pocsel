#ifndef __COMMON_POSITION__
#define __COMMON_POSITION__

#include "tools/Vector3.hpp"
#include "common/BaseChunk.hpp"

namespace Common {

    struct CubePosition;

    typedef Tools::Vector3d Position;

    /* coordonnées entières du chunk situé à cette position */
    BaseChunk::CoordsType GetChunkCoords(Position const& pos);

    /* position de l'origine du chunk situé à ces coordonnées entières */
    Position GetChunkPosition(BaseChunk::CoordsType const& chunkCoords);

    /* coordonnées entières du cube situé à cette position (non relatif au chunk) */
    BaseChunk::CoordsType GetCubeCoords(Position const& pos); // TODO

    /* coordonnées entières du cube relatives au chunk situé à cette position */
    BaseChunk::CoordsType GetCubeCoordsInChunk(Position const& pos);

    /* position du cube situé à ce CubePosition */
    Position GetCubePosition(CubePosition const& cubePos);

    /* position du cube relative au chunk situé à cette position */
    Position GetCubePositionInChunk(Position const& pos);

}

#endif
