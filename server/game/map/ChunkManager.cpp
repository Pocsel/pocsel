#include "server/game/map/ChunkManager.hpp"
#include "server/game/map/Map.hpp"

#include "tools/database/IConnection.hpp"
#include "tools/database/IValue.hpp"

#include "server/network/ChunkSerializer.hpp"

namespace Server { namespace Game { namespace Map {

    ChunkManager::ChunkManager(Map& map,
            std::vector<Chunk::IdType> const& existingBigChunks) :
        _map(map),
        _chunks(5000),
        _inflatedChunks(20000),
        _inflatedChunksContainers(50),
        _inflatedBigChunks(50),
        _dbBigChunks(100000)
    {
        Tools::debug << existingBigChunks.size() << " existing chunks in " << map.GetName() << "\n";

        std::for_each(existingBigChunks.begin(), existingBigChunks.end(), [this](Chunk::IdType const& id)
                {
                    this->_dbBigChunks.insert(id);
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

        for (auto it = this->_inflatedChunks.begin(), ite = this->_inflatedChunks.end(); it != ite; ++it)
        std::vector<Chunk::IdType> ids;

        for (auto it = this->_chunks.begin(), ite = this->_chunks.end(); it != ite; ++it)
            ids.push_back(it->first);

        for (auto it = ids.begin(), ite = ids.end(); it != ite; ++it)
            this->_InflateChunk(*it);

        ids.clear();

        for (auto it = this->_inflatedChunksContainers.begin(), ite = this->_inflatedChunksContainers.end(); it != ite; ++it)
        {
            if (it->second.IsFull())
                ids.push_back(it->first);
        }

        for (auto it = ids.begin(), ite = ids.end(); it != ite; ++it)
            this->_InflateBigChunk(*it);

        std::string query = Tools::ToString("REPLACE INTO ") + this->_map.GetName() +
            "_bigchunk (id, data) VALUES (?, ?)";

        curs.Execute("BEGIN");

        for (auto it = this->_inflatedBigChunks.begin(), ite = this->_inflatedBigChunks.end(); it != ite; ++it)
        {
            Tools::Database::Blob blob(it->second->GetData(), it->second->GetSize());
            curs.Execute(query)
//                    "REPLACE INTO ? (id, data) VALUES (?, ?)")
//                .Bind(this->_map.GetName() + "_chunk")
                .Bind(it->first)
                .Bind(blob);
        }

        curs.Execute("COMMIT");

        std::string query2 = Tools::ToString("REPLACE INTO ") + this->_map.GetName() +
            "_chunk (id, data) VALUES (?, ?)";

        curs.Execute("BEGIN");
        for (auto it = this->_inflatedChunks.begin(), ite = this->_inflatedChunks.end(); it != ite; ++it)
        {

            Tools::Database::Blob blob(it->second->GetData(), it->second->GetSize());
            curs.Execute(
                    query2.c_str())
                .Bind(it->first)
                .Bind(blob);
        }
        curs.Execute("COMMIT");
    }

    void ChunkManager::LoadExistingChunks(std::vector<Chunk::IdType> const& ids)
    {
        auto conn = this->_map.GetConnection();

        auto& curs = conn->GetCursor();

        std::string query = "SELECT data FROM ";
        query += this->_map.GetName() + "_chunk";
        query += " WHERE id = ?";

        for (auto it = ids.begin(), ite = ids.end(); it != ite; ++it)
        {
            curs.Execute(query.c_str()).Bind(*it);

            auto& row = curs.FetchOne();
            Tools::Database::Blob b = row[0].GetBlob();

            Tools::ByteArray* inflatedChunk = new Tools::ByteArray();
            inflatedChunk->SetData((char*)b.data, b.size);

            this->_inflatedChunks[*it] = inflatedChunk;

            BigChunk::IdType bigId = BigChunk::GetId(*it);
            if (this->_inflatedChunksContainers.count(bigId) == 0)
                this->_inflatedChunksContainers.insert(
                        std::pair<BigChunk::IdType, BigChunk>(bigId, BigChunk(bigId)));

            BigChunk& bigChunk = this->_inflatedChunksContainers.find(bigId)->second;
            bigChunk.AddChunk(*it);
            //        if (bigChunk.IsFull())
            //            this->_inflatedChunksContainers.erase(BigChunk::GetId(id));
        }

        curs.Execute((std::string("DELETE FROM ") + this->_map.GetName() + "_chunk").c_str());
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
                    if (this->_dbBigChunks.count(BigChunk::GetId(id)) == 0)
                        return 0;

                    this->_ExtractFromDb(id);
                }
                else
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
        assert(this->_dbBigChunks.count(BigChunk::GetId(id)) == 0);

        Chunk* chunk = this->_chunks[id];

        Tools::ByteArray* array = new Tools::ByteArray();
        array->Write(*chunk);
        array->ResetOffset();

        this->_inflatedChunks[id] = array;
        this->_chunks.erase(id);
        Tools::Delete(chunk);

        BigChunk::IdType bigId = BigChunk::GetId(id);
        if (this->_inflatedChunksContainers.count(bigId) == 0)
            this->_inflatedChunksContainers.insert(
                    std::pair<BigChunk::IdType, BigChunk>(bigId, BigChunk(bigId)));

        BigChunk& bigChunk = this->_inflatedChunksContainers.find(bigId)->second;
        bigChunk.AddChunk(id);
//        if (bigChunk.IsFull())
//            this->_inflatedChunksContainers.erase(BigChunk::GetId(id));
    }

    void ChunkManager::_DeflateChunk(Chunk::IdType id)
    {
        assert(this->_chunks.find(id) == this->_chunks.end());
        assert(this->_inflatedChunks.find(id) != this->_inflatedChunks.end());
        assert(this->_dbBigChunks.count(BigChunk::GetId(id)) == 0);

        Tools::ByteArray* array = this->_inflatedChunks[id];

        Chunk* chunk = array->Read<Chunk>().release();

        this->_chunks[id] = chunk;
        this->_inflatedChunks.erase(id);
        Tools::Delete(array);

        BigChunk& bigChunk = this->_inflatedChunksContainers.find(BigChunk::GetId(id))->second;
        bigChunk.RemoveChunk(id);
        if (bigChunk.IsEmpty())
            this->_inflatedChunksContainers.erase(BigChunk::GetId(id));
    }

    void ChunkManager::_InflateBigChunk(BigChunk::IdType bigId)
    {
        Tools::ByteArray* inflatedBigChunk = new Tools::ByteArray();

        auto ids = BigChunk::GetContainedIds<0>(bigId);

        Tools::ByteArray* inflatedChunk;

        for (auto it = ids.begin(), ite = ids.end(); it != ite; ++it)
        {
            inflatedChunk = this->_inflatedChunks[*it];

            inflatedBigChunk->WriteRawData(inflatedChunk->GetData(), inflatedChunk->GetSize());


            this->_inflatedChunks.erase(*it);
            Tools::Delete(inflatedChunk);
        }

        std::cout << "3Chunk before compression: " << inflatedBigChunk->GetSize() << " bytes\n";

        std::cout << "3Chunk after compression: " << inflatedBigChunk->GetSize() << " bytes\n";

        this->_inflatedBigChunks[bigId] = inflatedBigChunk;
        this->_inflatedChunksContainers.erase(bigId);
    }

    // extract 1 gros chunks
    void ChunkManager::_ExtractFromDb(Chunk::IdType id)
    {
        assert(this->_chunks.find(id) == this->_chunks.end());
        assert(this->_inflatedChunks.find(id) == this->_inflatedChunks.end());
        assert(this->_dbBigChunks.count(BigChunk::GetId(id)) == 1);

        id = BigChunk::GetId(id);

        auto conn = this->_map.GetConnection();

        auto& curs = conn->GetCursor();

        std::string query = "SELECT data FROM ";
        query += this->_map.GetName() + "_bigchunk";
        query += " WHERE id = ?";
        curs.Execute(query.c_str()).Bind(id);

        auto& row = curs.FetchOne();
        Tools::Database::Blob b = row[0].GetBlob();

        Tools::ByteArray* inflatedBigChunk = new Tools::ByteArray();
        inflatedBigChunk->SetData((char*)b.data, b.size);

        Chunk* chunk;


        for (unsigned int i = 0; i < BigChunk::chunkCount3; ++i)
        {
            chunk = inflatedBigChunk->Read<Chunk>().release();
            this->_chunks[chunk->id] = chunk;
        }

        Tools::Delete(inflatedBigChunk);
        this->_dbBigChunks.erase(id);
    }

}}}
