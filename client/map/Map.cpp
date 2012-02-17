#include "client/precompiled.hpp"

#include "client/game/Game.hpp"
#include "client/map/Map.hpp"

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