#include "server/game/map/ChunkManager.hpp"

#include "server/network/ChunkSerializer.hpp"

namespace Server { namespace Game { namespace Map {

    ChunkManager::ChunkManager()
    {
    }

    ChunkManager::~ChunkManager()
    {
        for (auto it = this->_chunks.begin(), ite = this->_chunks.end(); it != ite ; ++it)
            Tools::Delete(it->second);
        for (auto it = this->_inflatedChunks.begin(), ite = this->_inflatedChunks.end(); it != ite ; ++it)
            Tools::Delete(it->second);
    }

    Chunk* ChunkManager::GetChunk(Chunk::IdType id)
    {
        Chunk* chunk;

        {
            auto cit = this->_chunks.find(id);
            if (cit != this->_chunks.end())
                chunk = cit->second;
            else
            {
                auto ccit = this->_inflatedChunks.find(id);
                if (ccit == this->_inflatedChunks.end())
                    return 0;

                this->_DeflateChunk(id);

                chunk = this->_chunks[id];
            }
        }

        {
            std::list<Chunk::IdType> chunks;
            for (auto it = this->_chunks.begin(), ite = this->_chunks.end(); it != ite; ++it)
                chunks.push_back(it->first);

            for (auto it = chunks.begin(), ite = chunks.end(); it != ite; ++it)
            {
                if (*it != id)
                    this->_InflateChunk(*it);
            }
        }

        return chunk;
    }

    void ChunkManager::AddChunk(std::unique_ptr<Chunk> chunk)
    {
        Chunk::IdType id = chunk->id;

        this->_chunks[id] = chunk.release();

        this->_InflateChunk(id);
    }

    void ChunkManager::_InflateChunk(Chunk::IdType id)
    {
        assert(this->_chunks.find(id) != this->_chunks.end());
        assert(this->_inflatedChunks.find(id) == this->_inflatedChunks.end());

        Chunk* chunk = this->_chunks[id];

        Tools::ByteArray* array = new Tools::ByteArray();
        array->Write(*chunk);
        array->ResetOffset();

        this->_inflatedChunks[id] = array;
        this->_chunks.erase(id);
        Tools::Delete(chunk);
    }

    void ChunkManager::_DeflateChunk(Chunk::IdType id)
    {
        assert(this->_chunks.find(id) == this->_chunks.end());
        assert(this->_inflatedChunks.find(id) != this->_inflatedChunks.end());

        Tools::ByteArray* array = this->_inflatedChunks[id];

        Chunk* chunk = array->Read<Chunk>().release();

        this->_chunks[id] = chunk;
        this->_inflatedChunks.erase(id);
        Tools::Delete(array);
    }

}}}
