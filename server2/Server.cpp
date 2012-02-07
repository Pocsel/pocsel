#include "server2/Server.hpp"
#include "server2/Settings.hpp"
#include "server2/network/Network.hpp"
#include "server2/clientmanagement/ClientManager.hpp"
//#include "server2/Game.hpp"

namespace Server {

    Server::Server(int ac, char* av[]) :
        _settings(new Settings(ac, av)),
        _network(new Network::Network(*this)),
        _clientManager(new ClientManagement::ClientManager(*this))/*,
        _game(new Game(*this))
        */
    {
    }

    Server::~Server()
    {
        //Tools::Delete(this->_game);
        Tools::Delete(this->_clientManager);
        Tools::Delete(this->_network);
        Tools::Delete(this->_settings);
    }

    int Server::Run()
    {
        std::cout << "Running server2" << std::endl;

        this->_clientManager->Start();
        //this->_game->Start();
        this->_network->Run();

        return 0;
    }

    void Server::Stop()
    {
        std::cout << "Stopping server2" << std::endl;

//        this->_game->Stop();
        this->_clientManager->Stop();
        this->_network->Stop();
    }

}
