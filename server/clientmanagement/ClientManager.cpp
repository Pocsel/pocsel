#include "server/clientmanagement/ClientManager.hpp"
#include "server/clientmanagement/Client.hpp"
#include "server/clientmanagement/ClientActions.hpp"

#include "common/Packet.hpp"

#include "server/Server.hpp"

#include "server/game/Game.hpp"
#include "server/game/World.hpp"

#include "server/network/ClientConnection.hpp"
#include "server/network/PacketCreator.hpp"

namespace Server { namespace ClientManagement {

    ClientManager::ClientManager(Server& server) :
        Tools::SimpleMessageQueue(1),
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
        this->_Start();
    }

    void ClientManager::Stop()
    {
        Tools::debug << "ClientManager::Stop()\n";
        this->_Stop();
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

        //        client.Spawn(this->_game.GetWorld().GetDefaultMap());
        // TODO
        // spawn = créer le player, le foutre dans la game etc .. ?
    }

    void ClientManager::ClientNeedChunks(Client& client, std::vector<Chunk::IdType> const& ids)
    {
        std::function<void(Chunk const&)> callback =
            std::bind(&ClientManager::SendChunk, this, client.id, std::placeholders::_1);
        for (auto it = ids.begin(), ite = ids.end() ; it != ite ; ++it)
        {
            this->_server.GetGame().GetChunk(*it, client.id, callback);
        }
    }

    void ClientManager::ClientSpawn(Client& client)
    {
        this->_server.GetGame().SpawnPlayer(client.GetLogin(), client.id);
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
        clientConnection->SetCallbacks(std::bind(&ClientManager::HandleClientError, this, id),
                                       std::bind(&ClientManager::HandlePacket, this, id, std::placeholders::_1)
                                      );

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

        this->_clients[clientId]->SendPacket(std::move(Network::PacketCreator::TeleportPlayer(map, position)));

        // TODO passer le client en mode teleport (il n'existe plus dans le monde)
    }

}}
