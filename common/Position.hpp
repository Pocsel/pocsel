#ifndef __COMMON_POSITION__
#define __COMMON_POSITION__

#include "tools/Vector3.hpp"
#include "common/BaseChunk.hpp"
#include "common/CubePosition.hpp"

namespace Common {

    struct CubePosition;

    typedef Tools::Vector3d Position;

    /* coordonnées entières du chunk contenant cette position */
    inline BaseChunk::CoordsType GetChunkCoords(Position const& pos);

    /* position de l'origine du chunk situé à ces coordonnées entières */
    inline Position GetChunkPosition(BaseChunk::CoordsType const& chunkCoords);

    /* position de l'origine du chunk contenant cette position */
    inline Position GetChunkPosition(Position const& pos);

    /* coordonnées entières du cube contenant cette position (non relatif au chunk) */
    inline BaseChunk::CoordsType GetCubeCoords(Position const& pos);

    /* coordonnées entières du cube relatives au chunk contenant cette position */
    inline BaseChunk::CoordsType GetCubeCoordsInChunk(Position const& pos);

    /* position du cube situé à ce CubePosition (non relatif au chunk) */
    inline Position GetCubePosition(CubePosition const& cubePos);

    /* coordonnées entières du cube contenant cette position */
    inline CubePosition GetCubePosition(Position const& pos);

    /* position de l'origine du chunk contenant cette position */
    inline Position GetPositionInChunk(Position const& pos);

}

namespace Common {

    BaseChunk::CoordsType GetChunkCoords(Position const& pos)
    {
        return BaseChunk::CoordsType(pos.x, pos.y, pos.z) / Common::ChunkSize;
    }

    Position GetChunkPosition(BaseChunk::CoordsType const& chunkCoords)
    {
        return Position(chunkCoords.x, chunkCoords.y, chunkCoords.z) * Common::ChunkSize;
    }

    Position GetChunkPosition(Position const& pos)
    {
        BaseChunk::CoordsType coords = GetChunkCoords(pos);
        return Position(coords.x, coords.y, coords.z) * Common::ChunkSize;
    }

    BaseChunk::CoordsType GetCubeCoords(Position const& pos)
    {
        return BaseChunk::CoordsType(pos.x, pos.y, pos.z);
    }

    BaseChunk::CoordsType GetCubeCoordsInChunk(Position const& pos)
    {
        BaseChunk::CoordsType coords = GetChunkCoords(pos);
        return BaseChunk::CoordsType(
                pos.x - coords.x * Common::ChunkSize,
                pos.y - coords.y * Common::ChunkSize,
                pos.z - coords.z * Common::ChunkSize
                );
    }

    Position GetCubePosition(CubePosition const& cubePos)
    {
        Position world = GetChunkPosition(cubePos.world);
        return Position(
                world.x + cubePos.chunk.x,
                world.y + cubePos.chunk.y,
                world.z + cubePos.chunk.z
                );
    }

    CubePosition GetCubePosition(Position const& pos)
    {
        return CubePosition(GetChunkCoords(pos), GetCubeCoordsInChunk(pos));
    }

    Position GetPositionInChunk(Position const& pos)
    {
        return pos - GetChunkPosition(pos);
    }

}

#endif
