#ifndef __CLIENT_NETWORK_CHUNKSERIALIZER_HPP__
#define __CLIENT_NETWORK_CHUNKSERIALIZER_HPP__

#include "client/map/Chunk.hpp"

#include "tools/ByteArray.hpp"
#include "common/ChunkSerializer.hpp"


namespace Tools {

    template<>
    struct Tools::ByteArray::Serializer<Client::Map::Chunk>
        : public Common::ChunkSerializer<Client::Map::Chunk>
    {
    };

}

#endif
