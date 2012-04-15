#ifndef __COMMON_CUBEPOSITIONSERIALIZER_HPP__
#define __COMMON_CUBEPOSITIONSERIALIZER_HPP__

#include "tools/ByteArray.hpp"
#include "tools/VectorSerializer.hpp"

#include "common/CubePosition.hpp"

//namespace Common {
//
//    struct CubePositionSerializer
//    {
//        static std::unique_ptr<CubePosition> Read(Tools::ByteArray const& p)
//        {
//            BaseChunk::IdType chunkId;
//            glm::uvec3 positionInChunk;
//            p.Read(chunkId);
//            p.Read(positionInChunk);
//
//            return std::unique_ptr<CubePosition>(
//                    new CubePosition(BaseChunk::IdToCoords(chunkId), positionInChunk)
//                    );
//        }
//
//        static void Write(CubePosition const& pos, Tools::ByteArray& p)
//        {
//            p.Write(BaseChunk::CoordsToId(pos.world));
//            p.Write(pos.chunk);
//        }
//
//        static void Read(Tools::ByteArray const& p, CubePosition& pos)
//        {
//            BaseChunk::IdType chunkId;
//            p.Read(chunkId);
//            p.Read(pos.chunk);
//            pos.world = BaseChunk::IdToCoords(chunkId);
//        }
//
//    };
//
//}
//
//
//namespace Tools {
//
//    template<> struct ByteArray::Serializer< ::Common::CubePosition > :
//        public ::Common::CubePositionSerializer
//    {
//    };
//
//}

#endif
