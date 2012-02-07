#include "server2/clientmanagement/ClientManager.hpp"

namespace Server { namespace ClientManagement {

    ClientManager::ClientManager(Server& server) :
        Tools::SimpleMessageQueue(1),
        _server(server)
    {
    }

    ClientManager::~ClientManager()
    {
        // TODO
//        for (auto it = this->_clients.begin(), ite = this->_clients.end(); it != ite; ++it)
//            delete it->second;
    }

    void ClientManager::ManageNewClient(Network::ClientConnection* clientConnection)
    {

    }

}}
