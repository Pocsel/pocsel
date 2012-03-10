#ifndef __SERVER_GAME_MAP_BIGCHUNK_HPP__
#define __SERVER_GAME_MAP_BIGCHUNK_HPP__

#include "common/NChunkContainer.hpp"
#include "server/constants.hpp"

namespace Server { namespace Game { namespace Map {

    typedef Common::NChunkContainer<BigChunkSize, 0> BigChunk;

}}}

#endif
