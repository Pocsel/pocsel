#ifndef __SERVER_GAME_MAP_MAP_HPP__
#define __SERVER_GAME_MAP_MAP_HPP__

#include "server/game/map/Conf.hpp"

#include "server/game/map/Chunk.hpp"
#include "server/game/map/BigChunk.hpp"
#include "common/Position.hpp"
#include "common/CubePosition.hpp"

namespace Common {
    class Packet;
    struct MovingOrientedPosition;
    struct CastChunk;
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
        enum {
            SaveTime = 600,
            SaveRetryTime = 10
        };
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
        Uint64 _currentTime;

    public:
        Map(Conf const& conf,
                Uint64 currentTime,
                Game& game,
                std::vector<Chunk::IdType> const& existingBigChunks);
        ~Map();

        void Start();
        void Stop();
        void Save();

        std::string const& GetName() const { return this->_conf.name; }
        std::string const& GetFullName() const { return this->_conf.fullname; }
        Engine::Engine& GetEngine() { return *this->_engine; }
        Game& GetGame() { return this->_game; }

        // threadsafe
        void HandleNewChunk(Chunk* chunk);
        void GetSpawnPosition(SpawnCallback& response);
        void GetChunk(Chunk::IdType id, ChunkCallback& response);
        void GetChunkPacket(Chunk::IdType id, ChunkPacketCallback& response);
        void DestroyCube(Common::CubePosition const& pos);
        //void DestroyCubes(std::vector<Common::CubePosition> const& pos);
        void DestroyCubes(std::vector<Common::CastChunk*> const& pos);
        void AddPlayer(std::shared_ptr<Player> const& p);
        void RemovePlayer(Uint32 id);
        void MovePlayer(Uint32 id, Common::MovingOrientedPosition const& pos);

        // rcon requests
        void RconGetEntities(std::function<void(std::string)> cb) const;

    private:
        void _HandleNewChunk(Chunk* newChunk);
        void _GetSpawnPosition(SpawnCallback& response);
        void _GetChunk(Chunk::IdType id, ChunkCallback& response);
        void _SendChunkPacket(Chunk* chunk, ChunkPacketCallback& response);
        void _FindSpawnPosition(Chunk* chunk);
        void _AddPlayer(std::shared_ptr<Player> p);
        void _RemovePlayer(Uint32 id);
        void _MovePlayer(Uint32 id, Common::MovingOrientedPosition pos);
        void _DestroyCube(Chunk* chunk, Chunk::CoordsType cubePos);
        void _DestroyCubes(Chunk* chunk, std::vector<Chunk::CoordsType> cubePos);
        void _DestroyCubes2(Chunk* chunk, std::shared_ptr<Common::CastChunk> cubePos);
        void _DestroyChunk(Chunk::IdType id);
        void _DestroyChunkCallback(Chunk* chunk);
        void _PreDestroyCubes(std::shared_ptr<std::vector<Common::CastChunk*>> cubePos);
        void _SendChunkToPlayers(Chunk* chunk);
        void _Tick(Uint64 currentTime);
        void _TimedSave();

        // rcon requests
        void _RconGetEntities(std::function<void(std::string)> cb) const;
    };

}}}

#endif
