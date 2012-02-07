#ifndef __SERVER_CLIENTMANAGEMENT_CLIENTMANAGER_HPP__
#define __SERVER_CLIENTMANAGEMENT_CLIENTMANAGER_HPP__

#include "tools/SimpleMessageQueue.hpp"

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

    public:
        ClientManager(Server& server);
        ~ClientManager();

        void ManageNewClient(Network::ClientConnection* clientConnection);

        void Start() { this->_Start(); }
        void Stop() { this->_Stop(); }
    };

}}

#endif
