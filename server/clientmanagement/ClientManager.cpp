#include "server/clientmanagement/ClientManager.hpp"
#include "server/clientmanagement/Client.hpp"
#include "server/clientmanagement/ClientActions.hpp"

#include "common/Packet.hpp"
#include "common/Position.hpp"

#include "tools/SimpleMessageQueue.hpp"

#include "server/Server.hpp"

#include "server/game/Game.hpp"
#include "server/game/World.hpp"

#include "server/network/ClientConnection.hpp"
#include "server/network/PacketCreator.hpp"

namespace Server { namespace ClientManagement {

    ClientManager::ClientManager(Server& server, Tools::SimpleMessageQueue& messageQueue) :
        _messageQueue(messageQueue),
        _server(server),
        _nextId(1)
    {
        Tools::debug << "ClientManager::ClientManager()\n";
    }

    ClientManager::~ClientManager()
    {
        Tools::debug << "ClientManager::~ClientManager()\n";

        for (auto it = this->_clients.begin(), ite = this->_clients.end(); it != ite; ++it)
            Tools::Delete(it->second);
    }

    void ClientManager::Start()
    {
        Tools::debug << "ClientManager::Start()\n";
    }

    void ClientManager::Stop()
    {
        Tools::debug << "ClientManager::Stop()\n";
    }

    void ClientManager::HandleNewClient(boost::shared_ptr<Network::ClientConnection> clientConnection)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&ClientManager::_HandleNewClient, this, clientConnection));
        this->_messageQueue.PushMessage(m);
    }

    void ClientManager::HandleClientError(Uint32 clientId)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&ClientManager::_HandleClientError, this, clientId));
        this->_messageQueue.PushMessage(m);
    }

    void ClientManager::HandlePacket(Uint32 clientId, std::unique_ptr<Common::Packet>& packet)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&ClientManager::_HandlePacket, this, clientId, packet.release()));
        this->_messageQueue.PushMessage(m);
    }

    void ClientManager::HandleUdpPacket(std::unique_ptr<Common::Packet>& packet)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&ClientManager::_HandleUdpPacket, this, packet.release()));
        this->_messageQueue.PushMessage(m);
    }

    void ClientManager::SendPacket(Uint32 clientId, std::unique_ptr<Common::Packet> packet)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&ClientManager::_SendPacket, this, clientId, packet.release()));
        this->_messageQueue.PushMessage(m);
    }

    void ClientManager::SendChunk(Uint32 clientId, Chunk const& chunk)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&ClientManager::_SendChunk, this, clientId, std::cref(chunk)));
        this->_messageQueue.PushMessage(m);
    }

    void ClientManager::ClientTeleport(Uint32 clientId, std::string const& map, Common::Position const& position)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&ClientManager::_ClientTeleport, this, clientId, std::cref(map), position));
        this->_messageQueue.PushMessage(m);
    }

    Database::ResourceManager const& ClientManager::GetResourceManager() const
    {
        return this->_server.GetResourceManager();
    }

    Game::Game const& ClientManager::GetGame() const
    {
        return this->_server.GetGame();
    }

    void ClientManager::ClientLogin(Client& client, std::string const& login)
    {
        if (client.GetLogin() != "")
        {
            client.SendPacket(std::move(Network::PacketCreator::LoggedIn(false, "Already logged in")));
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
        client.SendPacket(std::move(Network::PacketCreator::LoggedIn(true,
            "",
            this->_server.GetGame().GetWorld().GetIdentifier(),
            this->_server.GetGame().GetWorld().GetFullname(),
            this->_server.GetGame().GetWorld().GetVersion(),
            static_cast<Common::BaseChunk::CubeType>(this->_server.GetGame().GetWorld().GetCubeTypes().size()))
            ));

        Tools::log << "Client logged in: " << login2 << "\n";
    }

    void ClientManager::ClientNeedChunks(Client& client, std::vector<Chunk::IdType> const& ids)
    {
        std::function<void(Chunk const&)>
            callback(std::bind(&ClientManager::SendChunk, this, client.id, std::placeholders::_1));
        for (auto it = ids.begin(), ite = ids.end() ; it != ite ; ++it)
        {
            this->_server.GetGame().GetChunk(*it, client.id, callback);
        }
    }

    void ClientManager::ClientSpawn(Client& client, Uint32 viewDistance, std::string const& playerName)
    {
        client.SetPlayerName(playerName);
        this->_server.GetGame().SpawnPlayer(client.GetLogin(), client.id, playerName, viewDistance);
    }

    void ClientManager::ClientTeleportOk(Client& client)
    {
        if (client.teleportMode == false)
            return;
        client.teleportMode = false;
        this->_server.GetGame().PlayerTeleportOk(client.id);
    }

    Uint32 ClientManager::_GetNextId()
    {
        if (this->_nextId == 0)
            this->_nextId = 1;
        return this->_nextId++;
    }

    void ClientManager::_HandleNewClient(boost::shared_ptr<Network::ClientConnection> clientConnection)
    {
        Uint32 id = this->_GetNextId();
        while (this->_clients.find(id) != this->_clients.end())
        {
            Tools::log << "Client id " << id << " already taken.\n";
            id = this->_GetNextId();
        }

        Network::ClientConnection::ErrorCallback
            ecb(std::bind(&ClientManager::HandleClientError, this, id));
        Network::ClientConnection::PacketCallback
            pcb(std::bind(&ClientManager::HandlePacket, this, id, std::placeholders::_1));
        clientConnection->SetCallbacks(ecb, pcb);

        Client* newClient = new Client(id, clientConnection);

        this->_clients[id] = newClient;

        Tools::log << "New client: " << id << "\n";
    }

    void ClientManager::_HandleClientError(Uint32 clientId)
    {
        if (this->_clients.find(clientId) == this->_clients.end())
        {
            Tools::error << "ClientError: Client " << clientId << " not found.\n";
            return;
        }

        Tools::debug << "Removing client " << clientId << "\n";
        Tools::Delete(this->_clients[clientId]);
        this->_clients.erase(clientId);
        this->_server.GetGame().RemovePlayer(clientId);
        Tools::debug << "Removing client " << clientId << " done\n";
    }

    void ClientManager::_HandlePacket(Uint32 clientId, Common::Packet* packet)
    {
        std::unique_ptr<Common::Packet> autoDelete(packet);
        if (this->_clients.find(clientId) == this->_clients.end())
        {
            Tools::error << "HandlePacket: Client " << clientId << " not found.\n";
            return ;
        }

        Client& client = *this->_clients[clientId];
        try
        {
            ClientActions::HandleAction(*this, client, *packet);
        }
        catch (std::exception& e)
        {
            Tools::log << "HandlePacket: " << e.what() << " (client " << clientId << ")\n";
            this->_HandleClientError(clientId);
        }
    }

    void ClientManager::_HandleUdpPacket(Common::Packet* packet)
    {
        bool ok = false;
        Uint32 clientId;
        try
        {
            packet->Read(clientId);
            ok = true;
        }
        catch (std::exception& e)
        {
            Tools::log << "Could not read id in UDP packet.\n";
            Tools::Delete(packet);
        }
        if (ok)
        {
            this->_HandlePacket(clientId, packet);
        }
    }

    void ClientManager::_SendPacket(Uint32 clientId, Common::Packet* packet)
    {
        if (this->_clients.find(clientId) == this->_clients.end())
        {
            Tools::error << "SendPacket: Client " << clientId << " not found.\n";
            Tools::Delete(packet);
            return ;
        }

        this->_clients[clientId]->SendPacket(std::unique_ptr<Common::Packet>(packet));
    }

    void ClientManager::_SendChunk(Uint32 clientId, Chunk const& chunk)
    {
        if (this->_clients.find(clientId) == this->_clients.end())
        {
            return ;
        }

        this->_clients[clientId]->SendPacket(std::move(Network::PacketCreator::Chunk(chunk)));
    }

    void ClientManager::_ClientTeleport(Uint32 clientId, std::string const& map, Common::Position const& position)
    {
        if (this->_clients.find(clientId) == this->_clients.end())
        {
            Tools::error << "ClientTeleport: Client " << clientId << " not found.\n";
            return ;
        }

        Tools::debug << "ClientManager::_ClientTeleport\n";

        this->_clients[clientId]->teleportMode = true;
        this->_clients[clientId]->SendPacket(std::move(Network::PacketCreator::TeleportPlayer(map, position)));
    }

}}
