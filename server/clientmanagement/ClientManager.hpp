#ifndef __SERVER_CLIENTMANAGEMENT_CLIENTMANAGER_HPP__
#define __SERVER_CLIENTMANAGEMENT_CLIENTMANAGER_HPP__

#include "server/game/map/Chunk.hpp"
#include "common/Position.hpp"

namespace Common {
    class Packet;
}

namespace Tools {
    class ByteArray;
    class SimpleMessageQueue;
}

namespace Server {
    class Server;
    struct Chunk;
    namespace Database {
        class ResourceManager;
    }
    namespace Game {
        class Game;
    }
    namespace Network {
        class ClientConnection;
    }
}

namespace Server { namespace ClientManagement {

    class Client;

    class ClientManager :
        private boost::noncopyable
    {
    private:
        Tools::SimpleMessageQueue& _messageQueue;
        Server& _server;
        std::unordered_map<Uint32, Client*> _clients;
        Uint32 _nextId;

    public:
        ClientManager(Server& server, Tools::SimpleMessageQueue& messageQueue);
        ~ClientManager();

        void Start();
        void Stop();

        // A appeler d'un autre thread
        void HandleNewClient(boost::shared_ptr<Network::ClientConnection> clientConnection);
        void HandleClientError(Uint32 clientId);
        void HandlePacket(Uint32 clientId, std::unique_ptr<Tools::ByteArray>& packet);
        void HandleUdpPacket(std::unique_ptr<Tools::ByteArray>& packet);
        void SendPacket(Uint32 clientId, std::unique_ptr<Common::Packet>& packet);
        void SendUdpPacket(Uint32 clientId, std::unique_ptr<Common::Packet>& packet);
        void ClientTeleport(Uint32 clientId, std::string const& map, Common::Position const& position);

        // A appeler du thread clientmanagement
        Database::ResourceManager const& GetResourceManager() const;
        Server& GetServer() { return this->_server; }
        void ClientLogin(Client& client, std::string const& login);
        void ClientNeedChunks(Client& client, std::vector<Game::Map::Chunk::IdType> const& ids);
        void ClientSpawn(Client& client, Uint32 viewDistance, std::string const& playerName);
        void ClientTeleportOk(Client& client);

    private:
        Uint32 _GetNextId();
        void _HandleNewClient(boost::shared_ptr<Network::ClientConnection> clientConnection);
        void _HandleClientError(Uint32 clientId);
        void _HandlePacket(Uint32 clientId, std::shared_ptr<Tools::ByteArray> packet);
        void _HandleUdpPacket(std::shared_ptr<Tools::ByteArray> packet);
        void _SendPacket(Uint32 clientId, std::shared_ptr<Common::Packet> packet);
        void _SendUdpPacket(Uint32 clientId, std::shared_ptr<Common::Packet> packet);
        void _ClientTeleport(Uint32 clientId, std::string const& map, Common::Position const& position);
    };

}}

#endif
