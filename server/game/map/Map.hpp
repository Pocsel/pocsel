#ifndef __SERVER_GAME_MAP_MAP_HPP__
#define __SERVER_GAME_MAP_MAP_HPP__

#include "server/game/map/Conf.hpp"

#include "server/Chunk.hpp"

namespace Common {
    struct Packet;
    struct Position;
    struct CubePosition;
}

namespace Tools {
    class SimpleMessageQueue;
}

namespace Server { namespace Game {
    class Game;
    class Player;
    namespace Map { namespace Gen {
        class ChunkGenerator;
    }}
    namespace Engine {
        class Engine;
    }
    namespace Entities {
        class EntityManager;
    }
}}

namespace Server { namespace Game { namespace Map {

    class ChunkManager;

    class Map :
        private boost::noncopyable
    {
    public:
        typedef std::function<void(Chunk const&)> ChunkCallback;
        typedef std::function<void(std::unique_ptr<Common::Packet>)> ChunkPacketCallback;
        typedef std::function<void(Common::Position const& pos)> SpawnCallback;

    private:
        Conf _conf;
        Game& _game;
        Tools::SimpleMessageQueue* _messageQueue;

        Gen::ChunkGenerator* _gen;
        std::unordered_map<Chunk::IdType, std::vector<ChunkCallback>> _chunkRequests;
        std::vector<SpawnCallback> _spawnRequests;

        std::map<Uint32, std::shared_ptr<Player>> _players;
        Common::Position* _spawnPosition;
        Entities::EntityManager* _entityManager;
        Engine::Engine* _engine;
        ChunkManager* _chunkManager;

    public:
        Map(Conf const& conf, Game& game);
        ~Map();

        void Start();
        void Stop();

        std::string const& GetName() const { return this->_conf.name; }

        // threadsafe
        void HandleNewChunk(Chunk* chunk);
        void GetSpawnPosition(SpawnCallback& response);
        void GetChunk(Chunk::IdType id, ChunkCallback& response);
        void GetChunkPacket(Chunk::IdType id, ChunkPacketCallback& response);
        void AddPlayer(std::shared_ptr<Player> const& p);
        void RemovePlayer(Uint32 id);
        void DestroyCube(Common::CubePosition const& pos);

    private:
        void _HandleNewChunk(Chunk* newChunk);
        void _GetSpawnPosition(SpawnCallback& response);
        void _GetChunk(Chunk::IdType id, ChunkCallback& response);
        void _GetChunkPacket(Chunk::IdType id, ChunkPacketCallback& response);
        void _SendChunkPacket(Chunk const& chunk, ChunkPacketCallback& response);
        void _FindSpawnPosition(Chunk const& chunk);
        void _AddPlayer(std::shared_ptr<Player> p);
        void _RemovePlayer(Uint32 id);
        void _DestroyCube(Common::CubePosition pos);
    };

}}}

#endif
