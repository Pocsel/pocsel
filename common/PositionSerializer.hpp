#ifndef __COMMON_POSITIONSERIALIZER_HPP__
#define __COMMON_POSITIONSERIALIZER_HPP__

#include <memory>

#include "tools/ByteArray.hpp"
#include "tools/VectorSerializer.hpp"

#include "common/Position.hpp"

namespace Common {

    struct PositionSerializer
    {
        static std::unique_ptr<Position> Read(Tools::ByteArray const& p)
        {
            BaseChunk::IdType chunkId;
            Tools::Vector3f positionInChunk;
            p.Read(chunkId);
            p.Read(positionInChunk);

            return std::unique_ptr<Position>(
                    new Position(BaseChunk::IdToCoords(chunkId), positionInChunk)
                    );
        }

        static void Write(Position const& pos, Tools::ByteArray& p)
        {
            p.Write(BaseChunk::CoordsToId(pos.world));
            p.Write(pos.chunk);
        }

        static void Read(Tools::ByteArray const& p, Position& pos)
        {
            BaseChunk::IdType chunkId;
            p.Read(chunkId);
            p.Read(pos.chunk);
            pos.world = BaseChunk::IdToCoords(chunkId);
        }
    };

}


namespace Tools {

    template<> struct ByteArray::Serializer< ::Common::Position > :
        public ::Common::PositionSerializer
    {
    };

}

#endif
