#ifndef __COMMON_POSITION__
#define __COMMON_POSITION__

#include "tools/Vector3.hpp"
#include "common/BaseChunk.hpp"
#include "common/CubePosition.hpp"

namespace Common {

    typedef double PositionCoordType;

    typedef glm::detail::tvec3<PositionCoordType> Position;

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

    /* position de l'origine du chunk contenant cette position */
    inline Position GetPositionInChunk(Position const& pos);

}

namespace Common {

    BaseChunk::CoordsType GetChunkCoords(Position const& pos)
    {
        return BaseChunk::CoordsType(pos) / Common::ChunkSize;
    }

    Position GetChunkPosition(BaseChunk::CoordsType const& chunkCoords)
    {
        return Position(chunkCoords) * (double)Common::ChunkSize;
    }

    Position GetChunkPosition(Position const& pos)
    {
        return Position(GetChunkCoords(pos)) * (double)Common::ChunkSize;
    }

    BaseChunk::CoordsType GetCubeCoords(Position const& pos)
    {
        return BaseChunk::CoordsType(pos);
    }

    BaseChunk::CoordsType GetCubeCoordsInChunk(Position const& pos)
    {
        return BaseChunk::CoordsType(pos - Position(GetChunkCoords(pos)) * (double)Common::ChunkSize);
    }

    Position GetPositionInChunk(Position const& pos)
    {
        return pos - GetChunkPosition(pos);
    }

}

#endif
