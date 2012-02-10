#include "server2/game/Game.hpp"
#include "server2/game/World.hpp"

namespace Server { namespace Game {

    Game::Game(Server& server) :
        _server(server)
    {
        Tools::debug << "Game::Game()\n";
        this->_world = new World(server);
    }

    Game::~Game()
    {
        Tools::debug << "Game::~Game()\n";
        Tools::Delete(this->_world);
    }

    void Game::Start()
    {
        Tools::debug << "Game::Start()\n";
        this->_world->Start();
    }

    void Game::Stop()
    {
        Tools::debug << "Game::Stop()\n";
        this->_world->Stop();
    }

}}
