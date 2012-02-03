#include <iostream>
#include <stdexcept>
#include <boost/bind.hpp>

#include "tools/ToString.hpp"

#include "common/Packet.hpp"

#include "server/Chunk.hpp"
#include "server/Client.hpp"
#include "server/ClientManager.hpp"
#include "server/ClientActions.hpp"
#include "server/Game.hpp"
#include "server/PacketCreator.hpp"
#include "server/World.hpp"

namespace Server {

    ClientManager::ClientManager(Game& game) :
        Tools::MessageQueue<ClientManager>(this),
        _game(game),
        _clients()
    {
    }

    ClientManager::~ClientManager()
    {
        this->_clients.clear();
    }

    void ClientManager::Stop()
    {
        auto it = this->_clients.begin(), end = this->_clients.end();
        for (; it != end; ++it)
        {
            (*it)->Shutdown();
        }
        this->_clients.clear();
    }

    void ClientManager::HandleNewClient(SocketPtr& conn)
    {
        Client::HandlePacketCallback onPacket = boost::bind(&ClientManager::_HandleClientPacket, this, _1, _2);
        Client::HandleErrorCallback onError = boost::bind(&ClientManager::_HandleClientError, this, _1, _2);
        ClientPtr newClient(new Client(onPacket, onError));
        this->_clients.insert(newClient);
        newClient->Connect(conn);
    }

    void ClientManager::ClientLogin(ClientPtr& client, std::string const& login)
    {
    //    auto it = this->_clients.begin(), end = this->_clients.end();
    //    for (; it != end; ++it)
    //    {
    //        if ((*it)->GetLogin() == login)
    //        {
    //            auto p = PacketCreator::LoggedInPacket(false, "Already used login");
    //            client->SendPacket(std::move(p));
    //            return;
    //        }
    //    }
        client->Login(login);
        auto p = PacketCreator::LoggedInPacket(true,
                "",
                this->_game.GetWorld().GetIdentifier(),
                this->_game.GetWorld().GetFullname(),
                this->_game.GetWorld().GetVersion(),
                this->_game.GetWorld().GetCubeTypes().size());
        client->SendPacket(std::move(p));
        client->Spawn(this->_game.GetWorld().GetDefaultMap());
    }

    void ClientManager::ClientLogout(ClientPtr& client)
    {
        client->Logout();
    }

    //void ClientManager::_HandleClientError(ClientPtr& client, boost::system::error_code const& err)
    //{
    //    std::cerr << "Client will be removed " << err << std::endl;
    //    this->_clients.erase(client);
    //    client->Shutdown();
    //}
    //
    //void ClientManager::_HandleClientPacket(ClientPtr& client, Common::PacketPtr& packet)
    //{
    //    try
    //    {
    //        ClientActions::HandleAction(this->_game, client, packet);
    //    }
    //    catch (std::exception& err)
    //    {
    //        std::cerr << "_HandlePacket(): " << err.what() << std::endl;
    //        this->_clients.erase(client);
    //        client->Shutdown();
    //    }
    //}

    void ClientManager::_HandleClientError(ClientPtr& client, boost::system::error_code const& err)
    {
        this->PushCall(&ClientManager::__HandleClientError, client, err);
    }
    void ClientManager::__HandleClientError(ClientPtr client, boost::system::error_code const err)
    {
        std::cerr << "Client will be removed " << err << std::endl;
        this->_clients.erase(client);
        client->Shutdown();
    }

    void ClientManager::_HandleClientPacket(ClientPtr& client, Common::PacketPtr& packet)
    {
        this->PushCall(&ClientManager::__HandleClientPacket, client, packet.release());
    }
    void ClientManager::__HandleClientPacket(ClientPtr client, Common::Packet* packet)
    {
        try
        {
            ClientActions::HandleAction(this->_game, client, *packet);
        }
        catch (std::exception& err)
        {
            std::cerr << "_HandlePacket(): " << err.what() << std::endl;
            this->_clients.erase(client);
            client->Shutdown();
        }
        Tools::Delete(packet);
    }

}
