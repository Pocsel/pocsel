#include "server2/clientmanagement/ClientManager.hpp"
#include "server2/clientmanagement/Client.hpp"
#include "server2/clientmanagement/ClientActions.hpp"

#include "server2/network/PacketCreator.hpp"

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
            Tools::Delete(it->second);
    }

    void ClientManager::Start()
    {
        this->_Start();
    }

    void ClientManager::Stop()
    {
        this->_Stop();
    }

    Uint32 ClientManager::_GetNextId()
    {
        if (this->_nextId == 0)
            this->_nextId = 1;
        return this->_nextId++;
    }

    void ClientManager::_HandleNewClient(Network::ClientConnection* clientConnection)
    {
        Uint32 id = this->_GetNextId();
        while (this->_clients.find(id) != this->_clients.end())
        {
            std::cout << "Client id " << id << " already taken.\n";
            id = this->_GetNextId();
        }
        Client* newClient = new Client(id, clientConnection);
        this->_clients[id] = newClient;

        std::cout << "New client: " << id << "\n";
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

    void ClientManager::_SendPacket(Uint32 clientId, Common::Packet* packet)
    {
        std::unique_ptr<Common::Packet> autoDelete(packet);
        if (this->_clients.find(clientId) == this->_clients.end())
        {
            std::cout << "HandlePacket: Client " << clientId << " not found.\n";
            return ;
        }

        this->_clients[clientId]->SendPacket(packet);
    }

    void ClientManager::ClientLogin(Client& client, std::string const& login)
    {
        if (client.GetLogin() != "")
        {
            client.SendPacket(Network::PacketCreator::LoggedIn(false, "Already logged in"));
            return;
        }

        std::string login2 = login;

        for (auto it = this->_clients.begin(), ite = this->_clients.end(); it != ite; ++it)
        {
            if ((*it).second->GetLogin() == login2)
            {
                login2 += "_";
                it = this->_clients.begin();
                if ((*it).second->GetLogin() == login2)
                    login2 += "_";
            }
        }

        client.SetLogin(login2);
        client.SendPacket(Network::PacketCreator::LoggedIn(true/*,
                "",
                this->_game.GetWorld().GetIdentifier(),
                this->_game.GetWorld().GetFullname(),
                this->_game.GetWorld().GetVersion(),
                static_cast<Common::BaseChunk::CubeType>(this->_game.GetWorld().GetCubeTypes().size())*/)
                         );

//        client.Spawn(this->_game.GetWorld().GetDefaultMap());
        // TODO
        // spawn = créer le player, le foutre dans la game etc .. ?
    }
}}
