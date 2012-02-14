#include "client2/precompiled.hpp"
#include "client2/game/Game.hpp"
#include "client2/map/ChunkManager.hpp"

namespace Client { namespace Map {

    ChunkManager::ChunkManager(Game::Game& game)
        : _game(game)
    {
    }

    void ChunkManager::Update(Common::Position const& playerPosition)
    {
        // TODO: gerer le cache des chunks !
    }

}}
