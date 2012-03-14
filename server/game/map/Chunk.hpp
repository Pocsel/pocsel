#ifndef __SERVER_CHUNK_HPP__
#define __SERVER_CHUNK_HPP__

#include "common/BaseChunk.hpp"

namespace Server { namespace Game { namespace Map {

    struct Chunk : public Common::BaseChunk
    {
        explicit Chunk(IdType id) : BaseChunk(id) {}
        explicit Chunk(CoordsType const& coords) : BaseChunk(coords) {}
    };

}}}

#endif
