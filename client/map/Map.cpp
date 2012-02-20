#include "client/precompiled.hpp"

#include "client/game/Game.hpp"
#include "client/map/Map.hpp"

namespace Client { namespace Map {

    Map::Map(Game::Game& game, std::string const& name)
        : _game(game),
        _chunkManager(game),
        _name(name)
    {
    }

    float Map::GetLoadingProgression() const
    {
        return this->_chunkManager.GetLoadingProgression();
    }

}}