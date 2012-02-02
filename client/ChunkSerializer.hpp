#ifndef __CLIENT_CHUNKSERIALIZER_HPP__
#define __CLIENT_CHUNKSERIALIZER_HPP__

#include "client/Chunk.hpp"

#include "tools/ByteArray.hpp"
#include "common/ChunkSerializer.hpp"


namespace Tools {

    template<> struct ByteArray::Serializer< ::Client::Chunk > :
        public Common::ChunkSerializer< ::Client::Chunk >
        {
        };

}

#endif
