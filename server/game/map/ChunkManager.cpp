#include "server/precompiled.hpp"

#include "server/game/map/ChunkManager.hpp"
#include "server/game/map/Map.hpp"

#include "tools/database/IConnection.hpp"

#include "tools/zlib/Worker.hpp"

#include "server/network/ChunkSerializer.hpp"

namespace Server { namespace Game { namespace Map {

    ChunkManager::ChunkManager(Map& map,
            std::vector<Chunk::IdType> const& existingBigChunks) :
        _map(map),
        _chunks(5000),
        _deflatedChunks(20000),
        _deflatedChunksContainers(50),
        _deflatedBigChunks(50),
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
        for (auto it = this->_deflatedChunks.begin(), ite = this->_deflatedChunks.end(); it != ite ; ++it)
            Tools::Delete(it->second);
    }

    void ChunkManager::Save(Tools::Database::IConnection& conn)
    {
        {
            std::vector<Chunk::IdType> ids;
            for (auto it = this->_chunks.begin(), ite = this->_chunks.end(); it != ite; ++it)
                ids.push_back(it->first);
            for (auto it = ids.begin(), ite = ids.end(); it != ite; ++it)
                this->_MoveInflatedToDeflated(*it);
            ids.clear();
        }

        {
            std::vector<BigChunk::IdType> ids;
            for (auto it = this->_deflatedChunksContainers.begin(), ite = this->_deflatedChunksContainers.end(); it != ite; ++it)
                ids.push_back(it->first);
            for (auto it = ids.begin(), ite = ids.end(); it != ite; ++it)
                this->_MoveDeflatedToDeflatedBig(*it);
            ids.clear();
        }

        auto query = conn.CreateQuery(std::string("REPLACE INTO ") + this->_map.GetName() + "_bigchunk (id, data) VALUES (?, ?)");

        conn.BeginTransaction();
        for (auto it = this->_deflatedBigChunks.begin(), ite = this->_deflatedBigChunks.end(); it != ite; ++it)
            query->Bind(it->first).Bind((void const*)it->second->GetData(), it->second->GetSize()).ExecuteNonSelect().Reset();
        conn.EndTransaction();
    }

    Chunk* ChunkManager::GetChunk(Chunk::IdType id)
    {
        {
            // cherche dans les chunks decompresses
            if (this->_chunks.count(id) == 0)
            {
                // cherche dans les chunks compresses
                if (this->_deflatedChunks.count(id) == 0)
                {
                    // cherche dans les deflatedbigchunks
                    if (this->_deflatedBigChunks.count(BigChunk::GetId(id)) == 0)
                    {
                        // cherche dans la db
                        if (this->_dbBigChunks.count(BigChunk::GetId(id)) == 0)
                        {
                            return 0;
                        }
                        this->_MoveDbToDeflatedBig(BigChunk::GetId(id));
                    }
                    this->_MoveDeflatedBigToDeflated(BigChunk::GetId(id));
                }
                if (this->_deflatedChunks.count(id) == 0)
                    return 0;
                this->_MoveDeflatedToInflated(id);
            }
        }

        Chunk* chunk = this->_chunks[id];

        {
            std::list<Chunk::IdType> chunks;
            for (auto it = this->_chunks.begin(), ite = this->_chunks.end(); it != ite; ++it)
                chunks.push_back(it->first);

            for (auto it = chunks.begin(), ite = chunks.end(); it != ite; ++it)
            {
                if (*it != id)
                    this->_MoveInflatedToDeflated(*it);
            }
        }

        return chunk;
    }

    void ChunkManager::AddChunk(std::unique_ptr<Chunk> chunk)
    {
        Chunk::IdType id = chunk->id;

        //std::cout << chunk->IsEmpty() << "\n";

        this->_chunks[id] = chunk.release();

        this->_MoveInflatedToDeflated(id);
    }

    void ChunkManager::_MoveInflatedToDeflated(Chunk::IdType id)
    {
        assert(this->_chunks.count(id) == 1);
        assert(this->_deflatedChunks.count(id) == 0);
        assert(this->_deflatedChunksContainers.count(BigChunk::GetId(id)) == 0 || !this->_deflatedChunksContainers.find(BigChunk::GetId(id))->second.HasChunk(id));
        assert(this->_deflatedBigChunks.count(BigChunk::GetId(id)) == 0);
        assert(this->_dbBigChunks.count(BigChunk::GetId(id)) == 0);

        Chunk* chunk = this->_PopInflated(id);
        this->_PushDeflated(id, this->_DeflateChunk(*chunk));
        Tools::Delete(chunk);
    }

    void ChunkManager::_MoveDeflatedToDeflatedBig(BigChunk::IdType bigId)
    {
        assert(this->_deflatedChunksContainers.count(bigId) == 1);
        assert(this->_deflatedBigChunks.count(bigId) == 0);
        assert(this->_dbBigChunks.count(bigId) == 0);

        Tools::ByteArray bigChunk;

        auto ids = this->_deflatedChunksContainers.find(bigId)->second.GetContained();

        Tools::ByteArray* deflatedChunk;

        for (auto it = ids.begin(), ite = ids.end(); it != ite; ++it)
        {
            deflatedChunk = this->_PopDeflated(*it);
            bigChunk.WriteRawData(deflatedChunk->GetData(), deflatedChunk->GetSize());
            Tools::Delete(deflatedChunk);
        }

        this->_PushDeflatedBig(bigId, this->_DeflateBigChunk(bigChunk));
    }

    void ChunkManager::_MoveDeflatedBigToDb(BigChunk::IdType bigId)
    {
        //assert(this->_deflatedChunksContainers.count(bigId) == 0);
        assert(this->_deflatedBigChunks.count(bigId) == 1);
        assert(this->_dbBigChunks.count(bigId) == 0);

        Tools::ByteArray* deflatedBigChunk = this->_PopDeflatedBig(bigId);
        this->_PushDb(bigId, deflatedBigChunk);
        Tools::Delete(deflatedBigChunk);
    }

    void ChunkManager::_MoveDeflatedToInflated(Chunk::IdType id)
    {
        assert(this->_chunks.count(id) == 0);
        assert(this->_deflatedChunks.count(id) == 1);
        assert(this->_deflatedChunksContainers.count(BigChunk::GetId(id)) == 1 && this->_deflatedChunksContainers.find(BigChunk::GetId(id))->second.HasChunk(id));
        assert(this->_deflatedBigChunks.count(BigChunk::GetId(id)) == 0);
        assert(this->_dbBigChunks.count(BigChunk::GetId(id)) == 0);

        Tools::ByteArray* array = this->_PopDeflated(id);

        this->_chunks[id] = this->_InflateChunk(*array);

        Tools::Delete(array);
    }

    void ChunkManager::_MoveDeflatedBigToDeflated(BigChunk::IdType bigId)
    {
        //assert(this->_deflatedChunksContainers.count(bigId) == 0);
        assert(this->_deflatedBigChunks.count(bigId) == 1);
        assert(this->_dbBigChunks.count(bigId) == 0);

        Tools::ByteArray* deflatedBigChunk = this->_PopDeflatedBig(bigId);
        Tools::ByteArray* bigChunk = this->_InflateBigChunk(*deflatedBigChunk);
        Tools::Delete(deflatedBigChunk);

        while (bigChunk->GetBytesLeft() > 0)
        {
            Chunk* chunk = bigChunk->Read<Chunk>().release();
            this->_PushDeflated(chunk->id, this->_DeflateChunk(*chunk));
            Tools::Delete(chunk);
        }

        Tools::Delete(bigChunk);
    }

    void ChunkManager::_MoveDbToDeflatedBig(BigChunk::IdType bigId)
    {
        //assert(this->_deflatedChunksContainers.count(bigId) == 0);
        assert(this->_deflatedBigChunks.count(bigId) == 0);
        assert(this->_dbBigChunks.count(bigId) == 1);

        this->_PushDeflatedBig(bigId, this->_PopDb(bigId));
    }

    Tools::ByteArray* ChunkManager::_DeflateChunk(Chunk const& chunk)
    {
        Tools::ByteArray* deflatedChunk = new Tools::ByteArray();

        //std::cout << chunk.IsEmpty() << "\n";


        deflatedChunk->Write(chunk);

        deflatedChunk->ResetOffset();

        return deflatedChunk;
    }

    Chunk* ChunkManager::_InflateChunk(Tools::ByteArray const& array)
    {
        return array.Read<Chunk>().release();
    }

    Tools::ByteArray* ChunkManager::_DeflateBigChunk(Tools::ByteArray const& bigChunk)
    {
        char* deflatedData;
        unsigned int deflatedDataSize;

        Tools::Zlib::Worker w(6);
        w.Deflate(bigChunk.GetData(), bigChunk.GetSize(), (void*&)deflatedData, deflatedDataSize);

        Tools::ByteArray* deflatedBigChunk = new Tools::ByteArray();
        deflatedBigChunk->SetData(deflatedData, deflatedDataSize);

        Tools::Delete(deflatedData);

        std::cout << "3Chunk before compression: " << bigChunk.GetSize() << " bytes, "
            "3Chunk after compression: " << deflatedDataSize << " bytes\n";

        return deflatedBigChunk;
    }

    Tools::ByteArray* ChunkManager::_InflateBigChunk(Tools::ByteArray const& deflatedBigChunk)
    {
        char* inflatedData;
        unsigned int inflatedDataSize;

        Tools::Zlib::Worker w(6);
        w.Inflate(deflatedBigChunk.GetData(), deflatedBigChunk.GetSize(), (void*&)inflatedData, inflatedDataSize);

        Tools::ByteArray* bigChunk = new Tools::ByteArray();
        bigChunk->SetData(inflatedData, inflatedDataSize);

        Tools::Delete(inflatedData);
        return bigChunk;
    }

    void ChunkManager::_PushInflated(Chunk* chunk)
    {
        assert(this->_chunks.count(chunk->id) == 0);

        this->_chunks[chunk->id] = chunk;
    }

    Chunk* ChunkManager::_PopInflated(Chunk::IdType id)
    {
        assert(this->_chunks.count(id) == 1);

        Chunk* chunk = this->_chunks[id];
        this->_chunks.erase(id);

        //std::cout << chunk->IsEmpty() << "-\n";
        return chunk;
    }

    void ChunkManager::_PushDeflated(Chunk::IdType id, Tools::ByteArray* array)
    {
        assert(this->_deflatedChunks.count(id) == 0);

        this->_deflatedChunks[id] = array;

        BigChunk::IdType bigId = BigChunk::GetId(id);
        if (this->_deflatedChunksContainers.count(bigId) == 0)
            this->_deflatedChunksContainers.insert(
                    std::pair<BigChunk::IdType, BigChunk>(bigId, BigChunk(bigId)));

        BigChunk& bigChunk = this->_deflatedChunksContainers.find(bigId)->second;
        bigChunk.AddChunk(id);

        //if (bigChunk.IsFull())
        //    this->_MoveDeflatedToDeflatedBig(bigId);
    }

    Tools::ByteArray* ChunkManager::_PopDeflated(Chunk::IdType id)
    {
        assert(this->_deflatedChunks.count(id) == 1);
        assert(this->_deflatedChunksContainers.count(BigChunk::GetId(id)) == 1);

        Tools::ByteArray* array = this->_deflatedChunks[id];

        this->_deflatedChunks.erase(id);

        BigChunk::IdType bigId = BigChunk::GetId(id);

        BigChunk& bigChunk = this->_deflatedChunksContainers.find(bigId)->second;
        bigChunk.RemoveChunk(id);

        if (bigChunk.IsEmpty())
            this->_deflatedChunksContainers.erase(bigId);

        return array;
    }

    void ChunkManager::_PushDeflatedBig(BigChunk::IdType bigId, Tools::ByteArray* array)
    {
        assert(this->_deflatedBigChunks.count(bigId) == 0);

        this->_deflatedBigChunks[bigId] = array;
    }

    Tools::ByteArray* ChunkManager::_PopDeflatedBig(BigChunk::IdType bigId)
    {
        assert(this->_deflatedBigChunks.count(bigId) == 1);

        Tools::ByteArray* deflatedBigChunk = this->_deflatedBigChunks[bigId];
        this->_deflatedBigChunks.erase(bigId);
        return deflatedBigChunk;
    }

    void ChunkManager::_PushDb(BigChunk::IdType bigId, Tools::ByteArray* array)
    {
        assert(this->_dbBigChunks.count(bigId) == 0);

        auto& conn = this->_map.GetConnection();
        auto query = conn.CreateQuery("REPLACE INTO " + this->_map.GetName() + "_bigchunk (id, data) VALUES (?, ?)");
        query->Bind(bigId).Bind((void const*)array->GetData(), array->GetSize()).ExecuteNonSelect().Reset();

        this->_dbBigChunks.insert(bigId);
    }

    Tools::ByteArray* ChunkManager::_PopDb(BigChunk::IdType bigId)
    {
        assert(this->_dbBigChunks.count(bigId) == 1);

        auto& conn = this->_map.GetConnection();
        auto query = conn.CreateQuery("SELECT data FROM " + this->_map.GetName() + "_bigchunk WHERE id = ?");
        query->Bind(bigId);
        auto row = query->Fetch();
        auto b = row->GetArray(0);

        Tools::ByteArray* deflatedBigChunk = new Tools::ByteArray();
        deflatedBigChunk->SetData(b.data(), b.size());

        this->_dbBigChunks.erase(bigId);

        return deflatedBigChunk;
    }

}}}
