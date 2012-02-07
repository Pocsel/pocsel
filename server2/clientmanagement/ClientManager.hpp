#ifndef __SERVER_CLIENTMANAGEMENT_CLIENTMANAGER_HPP__
#define __SERVER_CLIENTMANAGEMENT_CLIENTMANAGER_HPP__

#include "tools/SimpleMessageQueue.hpp"

namespace Common {

    class Packet;

}

namespace Server {

    class Server;

    namespace Network {

        class ClientConnection;

    }
}

namespace Server { namespace ClientManagement {

    class Client;

    class ClientManager :
        public Tools::SimpleMessageQueue,
        private boost::noncopyable
    {
    private:
        Server& _server;
        std::unordered_map<Uint32, Client*> _clients;
        Uint32 _nextId;

    public:
        ClientManager(Server& server);
        ~ClientManager();

        void HandleNewClient(Network::ClientConnection* clientConnection)
        {
            this->_PushMessage(std::bind(&ClientManager::_HandleNewClient, this, clientConnection));
        }
        void HandleClientError(Uint32 clientId)
        {
            this->_PushMessage(std::bind(&ClientManager::_HandleClientError, this, clientId));
        }
        void HandlePacket(Uint32 clientId, Common::Packet* packet)
        {
            this->_PushMessage(std::bind(&ClientManager::_HandlePacket, this, clientId, packet));
        }

        void Start() { this->_Start(); }
        void Stop() { this->_Stop(); }
    private:
        Uint32 _GetNextId();
        void _HandleNewClient(Network::ClientConnection* clientConnection);
        void _HandleClientError(Uint32 clientId);
        void _HandlePacket(Uint32 clientId, Common::Packet* packet);
    };

}}

#endif
