#ifndef __SERVER_GAME_MAP_CHUNKMANAGER_HPP__
#define __SERVER_GAME_MAP_CHUNKMANAGER_HPP__

#include "tools/ByteArray.hpp"
#include "server/game/map/Chunk.hpp"
#include "server/game/map/BigChunk.hpp"

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
        struct ChunkValue
        {
            float value;
            Chunk::IdType id;

            ChunkValue(float v, Chunk::IdType i) :
                value(v), id(i)
            {
            }
            bool operator< (ChunkValue const& right)
            {
                return this->value < right.value;
            }
        };
    private:
        Map& _map;

        std::unordered_map<Chunk::IdType, Chunk*> _chunks;
        std::unordered_map<BigChunk::IdType, BigChunk> _inflatedChunksContainers;
        std::list<Chunk::IdType> _inflatedValues;

        std::unordered_map<Chunk::IdType, Tools::ByteArray*> _deflatedChunks;
        std::unordered_map<BigChunk::IdType, BigChunk> _deflatedChunksContainers;
        std::list<BigChunk::IdType> _deflatedValues;

        std::unordered_map<Chunk::IdType, Tools::ByteArray*> _deflatedBigChunks;
        std::list<BigChunk::IdType> _deflatedBigValues;

        std::unordered_set<Chunk::IdType> _dbBigChunks;

        std::list<std::pair<float, Chunk::IdType>> _priorities;

    public:
        ChunkManager(Map& map, std::vector<Chunk::IdType> const& existingChunks);
        ~ChunkManager();

        void Save(Tools::Database::IConnection& conn);

        Chunk* GetChunk(Chunk::IdType id);
        std::vector<Chunk*> GetChunks(std::vector<Chunk::IdType> const& id);

        void AddChunk(std::unique_ptr<Chunk> chunk);

    private:
        // _chunks
        // \/
        void _MoveInflatedToDeflated(Chunk::IdType id);
        // \/
        // _deflatedChunks
        // \/
        void _MoveDeflatedToDeflatedBig(BigChunk::IdType id);
        // \/
        // _deflatedBigChunks
        // \/
        void _MoveDeflatedBigToDb(BigChunk::IdType id);
        // \/
        // _dbBigChunks

        // _chunks
        // ./\.
        void _MoveDeflatedToInflated(Chunk::IdType id);
        // ./\.
        // _deflatedChunks
        // ./\.
        void _MoveDeflatedBigToDeflated(BigChunk::IdType id);
        // ./\.
        // _deflatedBigChunks
        // ./\.
        void _MoveDbToDeflatedBig(BigChunk::IdType id);
        // ./\.
        // _dbBigChunks

        static Tools::ByteArray* _DeflateChunk(Chunk const& chunk);
        static Chunk* _InflateChunk(Tools::ByteArray const& array);

        static Tools::ByteArray* _DeflateBigChunk(Tools::ByteArray const& bigChunk);
        static Tools::ByteArray* _InflateBigChunk(Tools::ByteArray const& array);

        void _PushInflated(Chunk* chunk);
        Chunk* _PopInflated(Chunk::IdType id);

        void _PushDeflated(Chunk::IdType, Tools::ByteArray* array);
        Tools::ByteArray* _PopDeflated(Chunk::IdType id);

        void _PushDeflatedBig(BigChunk::IdType, Tools::ByteArray* array);
        Tools::ByteArray* _PopDeflatedBig(BigChunk::IdType);

        void _PushDb(BigChunk::IdType, Tools::ByteArray* array);
        Tools::ByteArray* _PopDb(BigChunk::IdType);
    };

}}}

#endif
