#ifndef __SERVER_GAME_GAME_HPP__
#define __SERVER_GAME_GAME_HPP__

#include "tools/SimpleMessageQueue.hpp"

#include "server/Chunk.hpp"

namespace Common {

    struct Position;

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
        typedef std::function<void(Common::Position const&)> SpawnPlayerCallback;
        typedef std::function<void(Chunk const&)> ChunkCallback;

    private:
        Server& _server;
        Tools::SimpleMessageQueue _messageQueue;
        World* _world;
        std::unordered_map<Uint32, Player*> _players;

    public:
        Game(Server& server);
        ~Game();

        void Start();
        void Stop();

        World const& GetWorld() const { return *this->_world; }
        World& GetWorld() { return *this->_world; }

        void SpawnPlayer(std::string const& clientName, Uint32 clientId)
        {
            this->_messageQueue.PushMessage(std::bind(&Game::_SpawnPlayer, this, std::cref(clientName), clientId));
        }

        void PlayerTeleport(Uint32 id, Common::Position const& position)
        {
            this->_messageQueue.PushMessage(std::bind(&Game::_PlayerTeleport, this, id, std::cref(position)));
        }

        void GetChunk(Chunk::IdType id, Uint32 clientId, ChunkCallback callback)
        {
            this->_messageQueue.PushMessage(std::bind(&Game::_GetChunk, this, id, clientId, callback));
        }

    private:
        void _SpawnPlayer(std::string const& clientName, Uint32 clientId);
        void _PlayerTeleport(Uint32 id, Common::Position const& position);
        void _GetChunk(Chunk::IdType id, Uint32 clientId, ChunkCallback callback);
    };

}}

#endif
