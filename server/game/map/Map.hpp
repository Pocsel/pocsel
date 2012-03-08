#ifndef __SERVER_GAME_MAP_MAP_HPP__
#define __SERVER_GAME_MAP_MAP_HPP__

#include "server/game/map/Conf.hpp"

#include "server/Chunk.hpp"

namespace Common {
    class Packet;
    struct Position;
    struct CubePosition;
}

namespace Tools {
    class SimpleMessageQueue;
}

namespace Tools { namespace Database {
    class IConnection;
}}

namespace Server { namespace Game {
    class Game;
    class Player;
    namespace Map { namespace Gen {
        class ChunkGenerator;
    }}
    namespace Engine {
        class Engine;
    }
}}

namespace Server { namespace Game { namespace Map {

    class ChunkManager;

    class Map :
        private boost::noncopyable
    {
    public:
        typedef std::function<void(Chunk*)> ChunkCallback;
        typedef std::function<void(std::unique_ptr<Common::Packet>&)> ChunkPacketCallback;
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
        Engine::Engine* _engine;
        ChunkManager* _chunkManager;
        std::vector<Chunk::IdType> _existingChunks;

        Uint64 _currentTime;

    public:
        Map(Conf const& conf,
                Uint64 currentTime,
                Game& game,
                std::vector<Chunk::IdType> const& existingBigChunks,
                std::vector<Chunk::IdType> const& existingChunks);
        ~Map();

        void Start();
        void Stop();
        void Save();

        std::string const& GetName() const { return this->_conf.name; }
        Engine::Engine& GetEngine() { return *this->_engine; }
        Game& GetGame() { return this->_game; }

        std::shared_ptr<Tools::Database::IConnection> GetConnection();

        // threadsafe
        void HandleNewChunk(Chunk* chunk);
        void GetSpawnPosition(SpawnCallback& response);
        void GetChunk(Chunk::IdType id, ChunkCallback& response);
        void GetChunkPacket(Chunk::IdType id, ChunkPacketCallback& response);
        void DestroyCube(Common::CubePosition const& pos);
        void DestroyCubes(std::vector<Common::CubePosition> const& pos);
        void AddPlayer(std::shared_ptr<Player> const& p);
        void RemovePlayer(Uint32 id);

    private:
        void _HandleNewChunk(Chunk* newChunk);
        void _GetSpawnPosition(SpawnCallback& response);
        void _GetChunk(Chunk::IdType id, ChunkCallback& response);
        void _GenerateBigChunk(Chunk::IdType id);
        void _SendChunkPacket(Chunk* chunk, ChunkPacketCallback& response);
        void _FindSpawnPosition(Chunk* chunk);
        void _AddPlayer(std::shared_ptr<Player> p);
        void _RemovePlayer(Uint32 id);
        void _DestroyCube(Chunk* chunk, Chunk::CoordsType cubePos);
        void _DestroyCubes(Chunk* chunk, std::vector<Chunk::CoordsType> cubePos);
        void _SendChunkToPlayers(Chunk* chunk);
        void _Tick(Uint64 currentTime);
        void _GenerateUncompleteBigChunks();
    };

}}}

#endif
