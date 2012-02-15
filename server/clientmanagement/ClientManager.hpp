#ifndef __SERVER_CLIENTMANAGEMENT_CLIENTMANAGER_HPP__
#define __SERVER_CLIENTMANAGEMENT_CLIENTMANAGER_HPP__

#include "common/Packet.hpp"
#include "common/Position.hpp"
#include "tools/SimpleMessageQueue.hpp"

#include "server/Chunk.hpp"

namespace Common {

    class Packet;
    struct Position;

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

        // A appeler d'un autre thread
        void Start();
        void Stop();

        void HandleNewClient(boost::shared_ptr<Network::ClientConnection> clientConnection)
        {
            this->_PushMessage(std::bind(&ClientManager::_HandleNewClient, this, clientConnection));
        }
        void HandleClientError(Uint32 clientId)
        {
            this->_PushMessage(std::bind(&ClientManager::_HandleClientError, this, clientId));
        }
        void HandlePacket(Uint32 clientId, std::unique_ptr<Common::Packet>& packet)
        {
            this->_PushMessage(std::bind(&ClientManager::_HandlePacket, this, clientId, packet.release()));
        }
        void SendPacket(Uint32 clientId, std::unique_ptr<Common::Packet> packet)
        {
            this->_PushMessage(std::bind(&ClientManager::_SendPacket, this, clientId, packet.release()));
        }
        void SendChunk(Uint32 clientId, Chunk const& chunk)
        {
            this->_PushMessage(std::bind(&ClientManager::_SendChunk, this, clientId, std::cref(chunk)));
        }
        void ClientTeleport(Uint32 clientId, std::string const& map, Common::Position const& position)
        {
            this->_PushMessage(std::bind(&ClientManager::_ClientTeleport, this, clientId, map, position));
        }

        // A appeler du thread clientmanagement
        Database::ResourceManager const& GetResourceManager() const;
        Game::Game const& GetGame() const;
        void ClientLogin(Client& client, std::string const& login);
        void ClientNeedChunks(Client& client, std::vector<Chunk::IdType> const& ids);
        void ClientSpawn(Client& client);

    private:
        Uint32 _GetNextId();
        void _HandleNewClient(boost::shared_ptr<Network::ClientConnection> clientConnection);
        void _HandleClientError(Uint32 clientId);
        void _HandlePacket(Uint32 clientId, Common::Packet* packet);
        void _SendPacket(Uint32 clientId, Common::Packet* packet);
        void _SendChunk(Uint32 clientId, Chunk const& chunk);
        void _ClientTeleport(Uint32 clientId, std::string const& map, Common::Position const& position);
    };

}}

#endif
