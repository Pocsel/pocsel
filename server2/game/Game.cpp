#include "server2/game/Game.hpp"
#include "server2/game/World.hpp"

namespace Server { namespace Game {

    Game::Game(Server& server) :
        _server(server),
        _world(new World(server))
    {
    }

    Game::~Game()
    {
        Tools::Delete(this->_world);
    }

}}
