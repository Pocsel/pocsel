#include "server2/clientmanagement/ClientManager.hpp"
#include "server2/clientmanagement/Client.hpp"
#include "server2/clientmanagement/ClientActions.hpp"

#include "common/Packet.hpp"

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
        if (this->_clients.find(clientId) == this->_clients.end())
        {
            std::cout << "ClientError: Client " << clientId << " not found.\n";
            return;
        }

        std::cout << "Removing client " << clientId << "\n";
        this->_clients[clientId]->Shutdown();
        Tools::Delete(this->_clients[clientId]);
        this->_clients.erase(clientId);
    }

    void ClientManager::_HandlePacket(Uint32 clientId, Common::Packet* packet)
    {
        std::unique_ptr<Common::Packet> autoDelete(packet);
        if (this->_clients.find(clientId) == this->_clients.end())
        {
            std::cout << "HandlePacket: Client " << clientId << " not found.\n";
            return ;
        }

        Client& client = *this->_clients[clientId];
        try
        {
            ClientActions::HandleAction(*this, client, *packet);
        }
        catch (std::exception& e)
        {
            std::cout << "HandlePacket: " << e.what() << " (client " << clientId << ")\n";
            this->_HandleClientError(clientId);
        }
    }

}}
