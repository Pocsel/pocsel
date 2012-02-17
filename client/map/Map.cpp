#include "client2/precompiled.hpp"

#include "client2/game/Game.hpp"
#include "client2/map/Map.hpp"

namespace Client { namespace Map {

    Map::Map(Game::Game& game)
        : _game(game),
        _chunkManager(game)
    {
    }

    float Map::GetLoadingProgression() const
    {
        return this->_chunkManager.GetLoadingProgression();
    }

}}