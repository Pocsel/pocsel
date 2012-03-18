#include "server/clientmanagement/Client.hpp"

#include "common/Packet.hpp"

#include "server/network/ClientConnection.hpp"

namespace Server { namespace ClientManagement {

    Client::Client(Uint32 id,boost::shared_ptr<Network::ClientConnection> connection) :
        id(id),
        teleportMode(false),
        _connection(connection),
        _login("")
    {
        this->_connection->ConnectRead();
    }

    Client::~Client()
    {
        this->_connection->Shutdown();
    }

    void Client::SendPacket(std::unique_ptr<Common::Packet> packet)
    {
        this->_connection->SendPacket(std::move(packet));
    }

    void Client::SendUdpPacket(std::unique_ptr<Common::Packet> packet)
    {
        this->_connection->SendUdpPacket(std::move(packet));
    }

    void Client::SetLogin(std::string const& login)
    {
        assert(login.size() > 0 && "login is empty");
        this->_login = login;
    }

    void Client::SetPlayerName(std::string const& playerName)
    {
        assert(playerName.size() > 0 && "login is empty");
        this->_playerName = playerName;
    }

}}
