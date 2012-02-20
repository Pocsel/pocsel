#ifndef __SERVER_GAME_GAME_HPP__
#define __SERVER_GAME_GAME_HPP__

#include "server/Chunk.hpp"

namespace Common {

    struct Position;

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
        typedef std::function<void(Chunk const&)> ChunkCallback;

    private:
        Server& _server;
        Tools::SimpleMessageQueue& _messageQueue;
        World* _world;
        std::unordered_map<Uint32, Player*> _players;

    public:
        Game(Server& server, Tools::SimpleMessageQueue& messageQueue);
        ~Game();

        void Start();
        void Stop();

        World const& GetWorld() const { return *this->_world; }
        World& GetWorld() { return *this->_world; }

        // Thread safe
        void SpawnPlayer(std::string const& clientName, Uint32 clientId);
        void PlayerTeleport(Uint32 id, std::string const& map, Common::Position const& position);
        void GetChunk(Chunk::IdType id, Uint32 clientId, ChunkCallback callback);

    private:
        void _SpawnPlayer(std::string const& clientName, Uint32 clientId);
        void _PlayerTeleport(Uint32 id, std::string const& map, Common::Position const& position);
        void _GetChunk(Chunk::IdType id, Uint32 clientId, ChunkCallback callback);
    };

}}

#endif
