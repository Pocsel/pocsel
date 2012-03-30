#include "common/Position.hpp"

namespace Common {

    BaseChunk::CoordsType GetChunkCoords(Position const& pos)
    {
        return BaseChunk::CoordsType(
                pos.x / Common::ChunkSize,
                pos.y / Common::ChunkSize,
                pos.z / Common::ChunkSize
                );
    }

    Position GetChunkPosition(BaseChunk::CoordsType const& chunkCoords)
    {
        return Position(
                chunkCoords.x * Common::ChunkSize,
                chunkCoords.y * Common::ChunkSize,
                chunkCoords.z * Common::ChunkSize
                );
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

    Position GetCubePositionInChunk(Position const& pos)
    {
    }

}
