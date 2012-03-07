#include "server/game/map/ChunkManager.hpp"
#include "server/game/map/Map.hpp"

#include "tools/database/IConnection.hpp"
#include "tools/database/IValue.hpp"

#include "server/network/ChunkSerializer.hpp"

namespace Server { namespace Game { namespace Map {

    ChunkManager::ChunkManager(Map& map, std::vector<Chunk::IdType> const& existingChunks) :
        _map(map),
        _inflatedChunks(20000),
        _chunks(5000),
        _dbChunks(100000)
    {
        Tools::debug << existingChunks.size() << " existing chunks in " << map.GetName() << "\n";

        std::for_each(existingChunks.begin(), existingChunks.end(), [this](Chunk::IdType const& id)
                {
                    this->_dbChunks.insert(id);
                });
    }

    ChunkManager::~ChunkManager()
    {
        for (auto it = this->_chunks.begin(), ite = this->_chunks.end(); it != ite ; ++it)
            Tools::Delete(it->second);
        for (auto it = this->_inflatedChunks.begin(), ite = this->_inflatedChunks.end(); it != ite ; ++it)
            Tools::Delete(it->second);
    }

    void ChunkManager::Save(Tools::Database::IConnection& conn)
    {
        auto& curs = conn.GetCursor();

        std::string query = "REPLACE INTO " + this->_map.GetName() +
            "_chunk (id, data) VALUES (?, ?)";

        curs.Execute("BEGIN");
        for (auto it = this->_inflatedChunks.begin(), ite = this->_inflatedChunks.end(); it != ite; ++it)
        {
            Tools::Database::Blob blob(it->second->GetData(), it->second->GetSize());
            curs.Execute(query)
//                    "REPLACE INTO ? (id, data) VALUES (?, ?)")
//                .Bind(this->_map.GetName() + "_chunk")
                .Bind(it->first)
                .Bind(blob);
        }

        curs.Execute("COMMIT");
    }

    Chunk* ChunkManager::GetChunk(Chunk::IdType id)
    {
        Chunk* chunk;

        {
            // cherche dans les chunks decompresses
            auto cit = this->_chunks.find(id);
            if (cit != this->_chunks.end())
                chunk = cit->second;
            else
            {
                // cherche dans les chunks compresses
                auto ccit = this->_inflatedChunks.find(id);
                if (ccit == this->_inflatedChunks.end())
                {
                    // cherche dans la db
                    if (this->_dbChunks.count(id) == 0)
                        return 0;

                    this->_ExtractFromDb(id);
                }

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
        assert(this->_dbChunks.count(id) == 0);

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
        assert(this->_dbChunks.count(id) == 0);

        Tools::ByteArray* array = this->_inflatedChunks[id];

        Chunk* chunk = array->Read<Chunk>().release();

        this->_chunks[id] = chunk;
        this->_inflatedChunks.erase(id);
        Tools::Delete(array);
    }

    void ChunkManager::_ExtractFromDb(Chunk::IdType id)
    {
        assert(this->_chunks.find(id) == this->_chunks.end());
        assert(this->_inflatedChunks.find(id) == this->_inflatedChunks.end());
        assert(this->_dbChunks.count(id) == 1);

        auto conn = this->_map.GetConnection();

        auto& curs = conn->GetCursor();

        std::string query = "SELECT data FROM ";
        query += this->_map.GetName() + "_chunk";
        query += " WHERE id = ?";
        curs.Execute(query.c_str()).Bind(id);

        auto& row = curs.FetchOne();

        Tools::Database::Blob b = row[0].GetBlob();

        Tools::ByteArray* inflatedChunk = new Tools::ByteArray();
        inflatedChunk->SetData((char*)b.data, b.size);

        this->_inflatedChunks[id] = inflatedChunk;
        this->_dbChunks.erase(id);
    }

}}}
