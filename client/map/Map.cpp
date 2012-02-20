#include "client/precompiled.hpp"

#include "client/game/Game.hpp"
#include "client/map/Map.hpp"

namespace Client { namespace Map {

    Map::Map(Game::Game& game, std::string const& map)
        : _game(game),
        _chunkManager(game),
        _map(map)
    {
    }

    float Map::GetLoadingProgression() const
    {
        return this->_chunkManager.GetLoadingProgression();
    }

}}