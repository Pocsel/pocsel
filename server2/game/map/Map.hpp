#ifndef __SERVER_GAME_MAP_MAP_HPP__
#define __SERVER_GAME_MAP_MAP_HPP__

#include "server2/game/map/Conf.hpp"

#include "tools/SimpleMessageQueue.hpp"

#include "server2/Chunk.hpp"

namespace Common {

    struct Position;

}

namespace Server { namespace Game { namespace Map { namespace Gen {

    class ChunkGenerator;

}}}}

namespace Server { namespace Game { namespace Map {

    class Map :
        public Tools::SimpleMessageQueue,
        private boost::noncopyable
    {
    public:
        typedef std::function<void(Chunk const&)> ChunkCallback;
        typedef std::function<void(Common::Position const& pos)> SpawnCallback;

    private:
        Conf _conf;

        std::unordered_map<Chunk::IdType, Chunk*> _chunks;
        Gen::ChunkGenerator* _gen;
        std::unordered_map<Chunk::IdType, std::vector<ChunkCallback>> _chunkRequests;
        std::vector<SpawnCallback> _spawnRequests;

    public:
        Map(Conf const& conf);
        ~Map();

        void Start();
        void Stop();

        void GetChunk(Chunk::IdType id, ChunkCallback response)
        {
            this->_PushMessage(std::bind(&Map::_GetChunk, this, id, response));
        }

        void HandleNewChunk(Chunk* chunk)
        {
            this->_PushMessage(std::bind(&Map::_HandleNewChunk, this, chunk));
        }

        void GetSpawnPosition(SpawnCallback response)
        {
            this->_PushMessage(std::bind(&Map::_GetSpawnPosition, this, response));
        }

    private:
        void _GetChunk(Chunk::IdType id, ChunkCallback response);
        void _HandleNewChunk(Chunk* newChunk);
        void _GetSpawnPosition(SpawnCallback response);
//        void _FindSpawnPosition(Chunk const& chunk);
    };

}}}

#endif
