#include "server2/clientmanagement/ClientManager.hpp"
#include "server2/clientmanagement/Client.hpp"

namespace Server { namespace ClientManagement {

    ClientManager::ClientManager(Server& server) :
        Tools::SimpleMessageQueue(1),
        _server(server),
        _nextId(1)
    {
    }

    ClientManager::~ClientManager()
    {
        for (auto it = this->_clients.begin(), ite = this->_clients.end(); it != ite; ++it)
            delete it->second;
    }

    Uint32 ClientManager::_GetNextId()
    {
        if (this->_nextId == 0)
            this->_nextId = 1;
        return this->_nextId++;
    }

    void ClientManager::_HandleNewClient(Network::ClientConnection* clientConnection)
    {
        Client* newClient = new Client(this->_GetNextId(), clientConnection);
        this->_clients[newClient->id] = newClient;

        std::cout << "New client: " << newClient->id << "\n";
    }

    void ClientManager::_HandleClientError(Uint32 clientId)
    {
        std::cout << "Removing client: " << clientId << "\n";
        if (this->_clients.find(clientId) != this->_clients.end())
        {
            this->_clients[clientId]->Shutdown();
            Tools::Delete(this->_clients[clientId]);
            this->_clients.erase(clientId);
        }
        else
        {
            assert(false && "Ce client a deja ete delete");
        }
    }

    void ClientManager::_HandlePacket(Uint32 clientId, Common::Packet* packet)
    {
        std::cout << "PACKET RECEIVED\n";
    }

}}
