#include "server2/Server.hpp"
#include "server2/Settings.hpp"

#include "server2/network/Network.hpp"
#include "server2/database/ResourceManager.hpp"
#include "server2/clientmanagement/ClientManager.hpp"
#include "server2/game/Game.hpp"

namespace Server {

    Server::Server(int ac, char* av[]) :
        _settings(new Settings(ac, av)),
        _network(new Network::Network(*this)),
        _resourceManager(new Database::ResourceManager(*this)),
        _clientManager(new ClientManagement::ClientManager(*this)),
        _game(new Game::Game(*this))
    {
    }

    Server::~Server()
    {
        Tools::Delete(this->_game);
        Tools::Delete(this->_clientManager);
        Tools::Delete(this->_resourceManager);
        Tools::Delete(this->_network);
        Tools::Delete(this->_settings);
    }

    int Server::Run()
    {
        Tools::debug << "Running server2\n";

        this->_clientManager->Start();
        //this->_game->Start();
        this->_network->Run();

        return 0;
    }

    void Server::Stop()
    {
        Tools::debug << "Stopping server2\n";

//        this->_game->Stop();
        this->_clientManager->Stop();
        this->_network->Stop();
    }

}
