#ifndef __SERVER_GAME_MAP_CHUNKMANAGER_HPP__
#define __SERVER_GAME_MAP_CHUNKMANAGER_HPP__

#include "server/Chunk.hpp"
#include "tools/ByteArray.hpp"

namespace Tools { namespace Database {
    class IConnection;
}}

namespace Server { namespace Game { namespace Map {
    class Map;
}}}

namespace Server { namespace Game { namespace Map {

    class ChunkManager :
        private boost::noncopyable
    {
    private:
        Map& _map;
        std::unordered_map<Chunk::IdType, Tools::ByteArray*> _inflatedChunks;
        std::unordered_map<Chunk::IdType, Chunk*> _chunks;
        std::list<std::pair<float, Chunk::IdType>> _priorities;
        std::unordered_set<Chunk::IdType> _dbChunks;

    public:
        ChunkManager(Map& map, std::vector<Chunk::IdType> const& existingChunks);
        ~ChunkManager();

        void Save(Tools::Database::IConnection& conn);

        Chunk* GetChunk(Chunk::IdType id);
        std::vector<Chunk*> GetChunks(std::vector<Chunk::IdType> const& id);

        void AddChunk(std::unique_ptr<Chunk> chunk);

    private:
        void _InflateChunk(Chunk::IdType id);
        void _DeflateChunk(Chunk::IdType id);
        void _ExtractFromDb(Chunk::IdType id);
    };

}}}

#endif
