#ifndef __CLIENT_MAP_MAP_HPP__
#define __CLIENT_MAP_MAP_HPP__

#include "common/BaseChunk.hpp"
#include "client/map/ChunkManager.hpp"
#include "common/CubePosition.hpp"

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
        std::string _name;

    public:
        Map(Game::Game& game, std::string const& map);

        float GetLoadingProgression() const;
        ChunkManager& GetChunkManager() { return this->_chunkManager; }
        std::string const& GetName() const { return this->_name; }
        bool GetFirstCube(std::vector<Common::CubePosition> const& positions, Common::CubePosition& result);
    private:
    };

}}

#endif
