#ifndef __COMMON_POSITION_HPP__
#define __COMMON_POSITION_HPP__

#include "tools/Vector3.hpp"
#include "common/constants.hpp"
#include "common/BaseChunk.hpp"

namespace {

    inline void _JumpChunk(Common::BaseChunk::CoordType& world, float& chunk)
    {
        if (chunk < 0.0f || chunk >= ((float)Common::ChunkSize))
        {
            if (chunk < 0.0f)
            {
                world += ((int)chunk - (int)Common::ChunkSize) / ((int)Common::ChunkSize);
                chunk += (float)Common::ChunkSize;
            }
            else
                world += ((int)chunk + 1) / ((int)Common::ChunkSize);
            chunk -= (float)(((int)chunk) / ((int)Common::ChunkSize) * (int)Common::ChunkSize);
        }
    }

}

namespace Common {

    struct Position
    {
        BaseChunk::CoordsType world;
        Tools::Vector3f chunk;

        Position()
        {
        }

        Position(BaseChunk::CoordsType world, Tools::Vector3f chunk) :
            world(world), chunk(chunk)
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

        Tools::Vector3f GetOffset(Position const& pos) const
        {
            return ChunkSize*(Tools::Vector3f(world) - Tools::Vector3f(pos.world)) + (chunk - pos.chunk);
        }

        void AddOffset(Tools::Vector3f const& direction)
        {
            this->chunk += direction;
#define JUMP_CHUNK(XYZ) do { \
            if (this->chunk.XYZ < 0.0f || this->chunk.XYZ >= ((float)Common::ChunkSize)) \
            { \
                if (this->chunk.XYZ < 0.0f) \
                { \
                    this->world.XYZ += ((int)this->chunk.XYZ - (int)Common::ChunkSize) / ((int)Common::ChunkSize); \
                    this->chunk.XYZ += (float)Common::ChunkSize; \
                } \
                else \
                    this->world.XYZ += ((int)this->chunk.XYZ + 1) / ((int)Common::ChunkSize); \
                this->chunk.XYZ -= (float)(((int)this->chunk.XYZ) / ((int)Common::ChunkSize) * (int)Common::ChunkSize); \
            } \
            } while (0);

/*                if (chunk.XYZ < 0.0f) \
                    chunk.XYZ += (float)Common::ChunkSize; \*/

            //JUMP_CHUNK(x);
            //JUMP_CHUNK(y);
            //JUMP_CHUNK(z);
#undef JUMP_CHUNK
            _JumpChunk(this->world.x, this->chunk.x);
            _JumpChunk(this->world.y, this->chunk.y);
            _JumpChunk(this->world.z, this->chunk.z);
        }

        Position operator +(Position const& pos) const;
        Position operator -(Position const& pos) const;
        std::string Serialize() const;
        Position Clone() const;
    };

}

#endif
