#ifndef __COMMON_POSITION_HPP__
#define __COMMON_POSITION_HPP__

#include "tools/Vector3.hpp"
#include "constants.hpp"
#include "BaseChunk.hpp"

namespace Common {

    struct Position
    {
        BaseChunk::CoordsType world;
        Tools::Vector3f chunk;

        Position() {}
        Position(BaseChunk::CoordsType world, Tools::Vector3f chunk) :
            world(world),
            chunk(chunk)
        {
        }

        template<typename T>
        Tools::Vector3<T> GetVector() const
        {
            return Tools::Vector3<T>(
                T(world.x) * ChunkSize + T(chunk.x),
                T(world.y) * ChunkSize + T(chunk.y),
                T(world.z) * ChunkSize + T(chunk.z));
        }

        inline Tools::Vector3f operator -(Position const& pos) const
        {
            return ChunkSize*(Tools::Vector3f(world) - Tools::Vector3f(pos.world)) + (chunk - pos.chunk);
        }

        void operator += (Tools::Vector3f direction)
        {
            this->chunk += direction;
#define JUMP_CHUNK(XYZ) do { \
            if (this->chunk.XYZ < 0 || this->chunk.XYZ >= ((float)Common::ChunkSize)) \
            { \
                this->world.XYZ += ((int)this->chunk.XYZ) / ((int)Common::ChunkSize); \
                this->chunk.XYZ = std::fmod(this->chunk.XYZ, ((float)Common::ChunkSize)); \
            } \
            } while (0);

            JUMP_CHUNK(x);
            JUMP_CHUNK(y);
            JUMP_CHUNK(z);
#undef JUMP_CHUNK
        }
    };

}

#endif
