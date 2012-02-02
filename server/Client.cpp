
#include <cassert>
#include <iostream>
#include <boost/bind.hpp>

#include "common/Packet.hpp"

#include "tools/ToString.hpp"
#include "tools/Delete.hpp"

#include "server/ClientConnection.hpp"
#include "server/ClientActions.hpp"
#include "server/Client.hpp"
#include "server/PacketCreator.hpp"

namespace Server {

Client::Client(Client::HandlePacketCallback& onPacket, Client::HandleErrorCallback& onError) :
    Tools::MessageQueue<Client>(this),
    _conn(),
    _login(),
    _onPacket(onPacket),
    _onError(onError),
    _player(0)
{
}

Client::~Client()
{
    std::cout << "Deleting client " << Tools::ToString(this) << std::endl;
    this->_conn.reset();
    delete this->_player;
}

void Client::Shutdown()
{
    this->_conn->Shutdown();
}

void Client::Login(std::string const& login)
{
    assert(login.size() > 0 && "login is empty !");
    this->_login = login;
}

bool Client::IsLoggedIn() const
{
    return this->_login.size() > 0;
}

void Client::Logout()
{
    this->_login.clear();
}

void Client::Connect(SocketPtr& socket)
{
    assert(this->_conn == 0 && "Already connected");
    ClientConnection::HandlePacketCallback onPacket = boost::bind(this->_onPacket, this->shared_from_this(), _1);
    ClientConnection::HandleErrorCallback onError = boost::bind(this->_onError, this->shared_from_this(), _1);
    this->_conn = boost::shared_ptr<ClientConnection>(new ClientConnection(socket, onPacket, onError));
    this->_conn->ConnectRead();
}


void Client::SendChunk(Chunk const& chunk)
{
    Common::PacketPtr p(PacketCreator::ChunkPacket(chunk));
    this->SendPacket(std::move(p));
}

void Client::SpawnPosition(Common::Position const& pos)
{
    Common::PacketPtr p(PacketCreator::SpawnPosition(pos));
    this->SendPacket(std::move(p));
}

} // ns Server
