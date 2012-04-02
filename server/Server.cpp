#include "server/Server.hpp"
#include "server/Settings.hpp"

#include "tools/SimpleMessageQueue.hpp"

#include "server/network/Network.hpp"
#include "server/database/ResourceManager.hpp"
#include "server/clientmanagement/ClientManager.hpp"
#include "server/game/Game.hpp"

namespace Server {

    Server::Server(Settings& settings) :
        _settings(settings),
        _admMessageQueue(new Tools::SimpleMessageQueue(1))
    {
        Tools::debug << "Server::Server()\n";
        this->_resourceManager = new Database::ResourceManager(*this);
        this->_clientManager = new ClientManagement::ClientManager(*this, *this->_admMessageQueue);
        this->_game = new Game::Game(*this, *this->_admMessageQueue);

        Network::Network::NewConnectionHandler
            nch(std::bind(&ClientManagement::ClientManager::HandleNewClient,
                          this->_clientManager,
                          std::placeholders::_1,
                          std::placeholders::_2));
        Network::Network::PacketHandler
            ph(std::bind(&ClientManagement::ClientManager::HandlePacket,
                          this->_clientManager,
                          std::placeholders::_1,
                          std::placeholders::_2));
        Network::Network::ErrorHandler
            eh(std::bind(&ClientManagement::ClientManager::HandleClientError,
                          this->_clientManager,
                          std::placeholders::_1));
        this->_network = new Network::Network(*this, nch, ph, eh);
    }

    Server::~Server()
    {
        Tools::debug << "Server::~Server()\n";
        this->_game->Save();
        Tools::Delete(this->_game);
        Tools::Delete(this->_clientManager);
        Tools::Delete(this->_resourceManager);
        Tools::Delete(this->_network);
        Tools::Delete(this->_admMessageQueue);
    }

    int Server::Run()
    {
        Tools::debug << "Server::Run()\n";

        this->_clientManager->Start();
        this->_game->Start();
        this->_admMessageQueue->Start();
        this->_network->Run();

        return 0;
    }

    void Server::Stop()
    {
        Tools::debug << "Server::Stop()\n";

        this->_clientManager->Stop();
        this->_game->Stop();
        this->_admMessageQueue->Stop();
        this->_network->Stop();
    }

}
