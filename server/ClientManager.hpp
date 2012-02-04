#ifndef __SERVER_CLIENTMANAGER_HPP__
#define __SERVER_CLIENTMANAGER_HPP__

#include <set>

#include "tools/MessageQueue.hpp"

#include "common/PacketPtr.hpp"

#include "server/Chunk.hpp"
#include "server/SocketPtr.hpp"
#include "server/ClientPtr.hpp"

namespace Server {

    class Client;
    class Game;
    class PacketSender;

    class ClientManager :
        public Tools::MessageQueue<ClientManager>,
        private boost::noncopyable
    {
    private:
        Game& _game;
        std::set<ClientPtr> _clients;

    public:
        ClientManager(Game& game);
        ~ClientManager();
        void Stop();
        void HandleNewClient(SocketPtr& conn);
        inline size_t GetNbClient(void) const { return this->_clients.size(); }

        void ClientLogin(ClientPtr& client, std::string const& login);
        void ClientLogout(ClientPtr& client);

    private:
        void _HandleClientError(ClientPtr& client, boost::system::error_code const& error);
        void __HandleClientError(ClientPtr client, boost::system::error_code const error);
        void _HandleClientPacket(ClientPtr& client, Common::PacketPtr& packet);
        void __HandleClientPacket(ClientPtr client, Common::Packet* packet);
    };
}

#endif
