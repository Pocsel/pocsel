#ifndef __SERVER_GAME_GAME_HPP__
#define __SERVER_GAME_GAME_HPP__

#include "server/Chunk.hpp"

namespace Common {
    struct Position;
    struct Camera;
    struct CubePosition;
    class Packet;
}

namespace Tools {
    class SimpleMessageQueue;
}

namespace Server {
    class Server;
}

namespace Server { namespace Game {

    class World;
    class Player;

    class Game :
        private boost::noncopyable
    {
    public:
        typedef std::function<void(std::unique_ptr<Common::Packet>&)> ChunkPacketCallback;

    private:
        Server& _server;
        Tools::SimpleMessageQueue& _messageQueue;
        World* _world;
        std::unordered_map<Uint32, std::shared_ptr<Player>> _players;

    public:
        Game(Server& server, Tools::SimpleMessageQueue& messageQueue);
        ~Game();

        void Start();
        void Stop();

        World const& GetWorld() const { return *this->_world; }
        World& GetWorld() { return *this->_world; }
        Server const& GetServer() const { return this->_server; }
        Server& GetServer() { return this->_server; }

        void PlayerTeleportOk(Uint32 id);
        void PlayerAction(Uint32 id, Common::Camera const& cam, Common::CubePosition const& targetPos);

        // Thread safe
        void SpawnPlayer(std::string const& clientName, Uint32 clientId, std::string const& playerName, Uint32 viewDistance);
        void PlayerTeleport(Uint32 id, std::string const& map, Common::Position const& position);
        void GetChunkPacket(Chunk::IdType id, Uint32 clientId, ChunkPacketCallback& callback);
        void RemovePlayer(Uint32 clientId);

    private:
        void _SpawnPlayer(std::string const& clientName, Uint32 clientId, std::string const& playerName, Uint32 viewDistance);
        void _PlayerTeleport(Uint32 id, std::string const& map, Common::Position const& position);
        void _GetChunkPacket(Chunk::IdType id, Uint32 clientId, ChunkPacketCallback& callback);
        void _RemovePlayer(Uint32 clientId);
    };

}}

#endif
