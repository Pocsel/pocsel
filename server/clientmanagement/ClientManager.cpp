#include "server/clientmanagement/ClientManager.hpp"
#include "server/clientmanagement/Client.hpp"
#include "server/clientmanagement/ClientActions.hpp"

#include "common/Packet.hpp"
#include "common/Position.hpp"

#include "tools/SimpleMessageQueue.hpp"
#include "tools/Deleter.hpp"

#include "server/Server.hpp"

#include "server/game/Game.hpp"
#include "server/game/World.hpp"
#include "server/game/map/Map.hpp"
#include "server/game/engine/Engine.hpp"
#include "server/game/engine/BodyManager.hpp"

#include "server/network/ClientConnection.hpp"
#include "server/network/PacketCreator.hpp"

namespace Server { namespace ClientManagement {

    ClientManager::ClientManager(Server& server, Tools::SimpleMessageQueue& messageQueue) :
        _messageQueue(messageQueue),
        _server(server)
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

    void ClientManager::HandleNewClient(Uint32 id, boost::shared_ptr<Network::ClientConnection> clientConnection)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&ClientManager::_HandleNewClient, this, id, clientConnection));
        this->_messageQueue.PushMessage(m);
    }

    void ClientManager::HandleClientError(Uint32 clientId)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&ClientManager::_HandleClientError, this, clientId));
        this->_messageQueue.PushMessage(m);
    }

    void ClientManager::HandlePacket(Uint32 clientId, std::unique_ptr<Tools::ByteArray>& packet)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&ClientManager::_HandlePacket,
                        this,
                        clientId,
                        Tools::Deleter<Tools::ByteArray>::CreatePtr(packet.release())));
        this->_messageQueue.PushMessage(m);
    }

    void ClientManager::SendPacket(Uint32 clientId, std::unique_ptr<Common::Packet>& packet)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&ClientManager::_SendPacket,
                        this,
                        clientId,
                        Tools::Deleter<Common::Packet>::CreatePtr(packet.release())));
        this->_messageQueue.PushMessage(m);
    }

    void ClientManager::SendUdpPacket(Uint32 clientId, std::unique_ptr<Network::UdpPacket>& packet)
    {
        Tools::SimpleMessageQueue::Message
            m(std::bind(&ClientManager::_SendUdpPacket,
                        this,
                        clientId,
                        Tools::Deleter<Network::UdpPacket>::CreatePtr(packet.release())));
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
            client.id,
            this->_server.GetGame().GetWorld().GetIdentifier(),
            this->_server.GetGame().GetWorld().GetFullname(),
            this->_server.GetGame().GetWorld().GetVersion(),
            static_cast<Common::BaseChunk::CubeType>(this->_server.GetGame().GetWorld().GetDefaultMap().GetCubeTypes().size()),
            this->_server.GetGame().GetWorld().GetDefaultMap().GetEngine().GetBodyManager().GetNbBodyTypes(),
            this->_server.GetGame().GetWorld().GetBuildHash()
            )));

        Tools::log << "Client logged in: " << login2 << "\n";
    }

    void ClientManager::ClientNeedChunks(Client& client, std::vector<Game::Map::Chunk::IdType> const& ids)
    {
        Game::Game::ChunkPacketCallback
            callback(std::bind(&ClientManager::SendPacket, this, client.id, std::placeholders::_1));
        for (auto it = ids.begin(), ite = ids.end() ; it != ite ; ++it)
        {
            this->_server.GetGame().GetChunkPacket(*it, client.id, callback);
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

    void ClientManager::_HandleNewClient(Uint32 id, boost::shared_ptr<Network::ClientConnection> clientConnection)
    {
        if (this->_clients.find(id) != this->_clients.end())
        {
            Tools::log << "Client id " << id << " already taken.\n";
            clientConnection->Shutdown();
            return;
        }

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

    void ClientManager::_HandlePacket(Uint32 clientId, std::shared_ptr<Tools::ByteArray> packet)
    {
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

    void ClientManager::_SendPacket(Uint32 clientId, std::shared_ptr<Common::Packet> packet)
    {
        if (this->_clients.find(clientId) == this->_clients.end())
        {
            Tools::error << "SendPacket: Client " << clientId << " not found.\n";
            return ;
        }

        this->_clients[clientId]->SendPacket(std::unique_ptr<Common::Packet>(Tools::Deleter<Common::Packet>::StealPtr(packet)));
    }

    void ClientManager::_SendUdpPacket(Uint32 clientId, std::shared_ptr<Network::UdpPacket> packet)
    {
        if (this->_clients.find(clientId) == this->_clients.end())
        {
            Tools::error << "SendPacket: Client " << clientId << " not found.\n";
            return ;
        }

        this->_clients[clientId]->SendUdpPacket(std::unique_ptr<Network::UdpPacket>(Tools::Deleter<Network::UdpPacket>::StealPtr(packet)));
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
