#ifndef __SERVER_GAME_MAP_CHUNKMANAGER_HPP__
#define __SERVER_GAME_MAP_CHUNKMANAGER_HPP__

#include "server/Chunk.hpp"
#include "tools/ByteArray.hpp"

namespace Server { namespace Game { namespace Map {

    class ChunkManager :
        private boost::noncopyable
    {
    private:
        std::unordered_map<Chunk::IdType, Tools::ByteArray*> _inflatedChunks;
        std::unordered_map<Chunk::IdType, Chunk*> _chunks;
        std::list<std::pair<float, Chunk::IdType>> _priorities;

    public:
        ChunkManager();
        ~ChunkManager();

        Chunk* GetChunk(Chunk::IdType id);

        void AddChunk(std::unique_ptr<Chunk> chunk);

    private:
        void _InflateChunk(Chunk::IdType id);
        void _DeflateChunk(Chunk::IdType id);
    };

}}}

#endif