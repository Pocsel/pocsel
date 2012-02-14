#ifndef __CLIENT_MAP_CHUNKMANAGER_HPP__
#define __CLIENT_MAP_CHUNKMANAGER_HPP__

#include "common/BaseChunk.hpp"
#include "common/Position.hpp"

namespace Client { namespace Game {
    class Game;
}}

namespace Client { namespace Map {

    class ChunkManager
    {
    private:
        Game::Game& _game;
        std::deque<Common::BaseChunk::IdType> _downloadingChunks;

    public:
        ChunkManager(Game::Game& game);

        void Update(Common::Position const& playerPosition);
        float GetLoadingProgression() const { return 1.0f; } // TODO: gerer le bon truc ici
    private:
    };

}}

#endif
