#ifndef __SERVER_NETWORK_CHUNKSERIALIZER_HPP__
#define __SERVER_NETWORK_CHUNKSERIALIZER_HPP__

#include "server/game/map/Chunk.hpp"

#include "tools/ByteArray.hpp"

#include "common/ChunkSerializer.hpp"

namespace Tools {

    template<> struct ByteArray::Serializer< ::Server::Game::Map::Chunk > :
        public Common::ChunkSerializer< ::Server::Game::Map::Chunk >
    {
    };

}

#endif
