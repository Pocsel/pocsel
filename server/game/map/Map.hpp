#ifndef __SERVER_GAME_MAP_MAP_HPP__
#define __SERVER_GAME_MAP_MAP_HPP__

#include "server/game/map/Conf.hpp"

#include "server/Chunk.hpp"

namespace Common {

    struct Position;

}

namespace Tools {

    class SimpleMessageQueue;

}

namespace Server { namespace Game { namespace Map { namespace Gen {

    class ChunkGenerator;

}}}}

namespace Server { namespace Game { namespace Map {

    class Map :
        private boost::noncopyable
    {
    public:
        typedef std::function<void(Chunk const&)> ChunkCallback;
        typedef std::function<void(Common::Position const& pos)> SpawnCallback;

    private:
        Conf _conf;
        Tools::SimpleMessageQueue& _messageQueue;

        std::unordered_map<Chunk::IdType, Chunk*> _chunks;
        Gen::ChunkGenerator* _gen;
        std::unordered_map<Chunk::IdType, std::vector<ChunkCallback>> _chunkRequests;
        std::vector<SpawnCallback> _spawnRequests;
        Common::Position* _spawnPosition;

    public:
        Map(Conf const& conf, Tools::SimpleMessageQueue& gameMessageQueue);
        ~Map();

        void Start();
        void Stop();

        std::string const& GetName() const { return this->_conf.name; }

        // threadsafe
        void HandleNewChunk(Chunk* chunk);

        // Uniquement pour le thread Game
        void GetSpawnPosition(SpawnCallback& response);
        void GetChunk(Chunk::IdType id, ChunkCallback& response);

    private:
        void _HandleNewChunk(Chunk* newChunk);
        void _FindSpawnPosition(Chunk const& chunk);
    };

}}}

#endif