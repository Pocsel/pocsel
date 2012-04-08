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
        Tools::Database::IConnection& _conn;

        std::map<Chunk::IdType, Chunk*> _chunks;
        std::unordered_map<BigChunk::IdType, BigChunk> _inflatedChunksContainers;
        std::list<Chunk::IdType> _inflatedValues;

        std::map<Chunk::IdType, Tools::ByteArray*> _deflatedChunks;
        std::unordered_map<BigChunk::IdType, BigChunk> _deflatedChunksContainers;
        std::list<BigChunk::IdType> _deflatedValues;

        std::map<Chunk::IdType, Tools::ByteArray*> _deflatedBigChunks;
        std::list<BigChunk::IdType> _deflatedBigValues;

        std::set<Chunk::IdType> _dbBigChunks;

        std::list<std::pair<float, Chunk::IdType>> _priorities;

        static const int _deflateLevel = 9;
        static const unsigned int _maxInflated = 1000;
        static const unsigned int _maxDeflatedVal = 20;
        static const unsigned int _maxDeflatedBig = 40;

    public:
        ChunkManager(Map& map,
                Tools::Database::IConnection& conn,
                std::vector<Chunk::IdType> const& existingChunks);
        ~ChunkManager();

        void Save(Tools::Database::IConnection& conn);

        Chunk* GetChunk(Chunk::IdType id);
        std::vector<Chunk*> GetChunks(std::vector<Chunk::IdType> const& id);

        void AddChunk(std::unique_ptr<Chunk> chunk);

    private:
        void _DeflateIfPossible(Chunk::IdType noDeflateId = 0, bool noDeflate = false);

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
        void _MoveDeflatedBigToDb(BigChunk::IdType id); // does not push in db
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

        void _PushDb(BigChunk::IdType); // does not push in db
        Tools::ByteArray* _PopDb(BigChunk::IdType);
    };

}}}

#endif
