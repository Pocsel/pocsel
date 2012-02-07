#include "server2/clientmanagement/Client.hpp"

#include "server2/network/ClientConnection.hpp"

namespace Server { namespace ClientManagement {

    Client::Client(Uint32 id, Network::ClientConnection* connection) :
        id(id),
        _connection(connection)
    {
        this->_connection->SetClientId(id);
        this->_connection->ConnectRead();
    }

    Client::~Client()
    {
        this->_connection.reset();
    }

    void Client::Shutdown()
    {
        this->_connection->Shutdown();
    }

}}
