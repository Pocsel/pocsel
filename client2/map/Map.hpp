#ifndef __CLIENT_MAP_MAP_HPP__
#define __CLIENT_MAP_MAP_HPP__

#include "common/BaseChunk.hpp"
#include "client2/map/ChunkManager.hpp"

namespace Client {
    namespace Game {
        class Game;
    }
}

namespace Client { namespace Map {

    class Map
    {
    private:
        Game::Game& _game;
        ChunkManager _chunkManager;

    public:
        Map(Game::Game& game);

        float GetLoadingProgression() const;
    private:
    };

}}

#endif
