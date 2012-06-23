#include "server/precompiled.hpp"

#include "server/game/map/ChunkManager.hpp"
#include "server/game/map/Map.hpp"

#include "tools/database/IConnection.hpp"

#include "tools/zlib/Worker.hpp"
#include "tools/Timer.hpp"

#include "server/network/ChunkSerializer.hpp"

namespace Server { namespace Game { namespace Map {

    ChunkManager::ChunkManager(Map& map, Tools::Database::IConnection& conn,
            std::vector<Chunk::IdType> const& existingBigChunks) :
        _map(map),
        _conn(conn)
    {
        Tools::debug << existingBigChunks.size() << " existing chunks in " << map.GetName() << "\n";

        for (auto it = existingBigChunks.begin(), ite = existingBigChunks.end(); it != ite; ++it)
            this->_dbBigChunks.insert(*it);
    }

    ChunkManager::~ChunkManager()
    {
        for (auto it = this->_chunks.begin(), ite = this->_chunks.end(); it != ite ; ++it)
            Tools::Delete(it->second);
        for (auto it = this->_deflatedChunks.begin(), ite = this->_deflatedChunks.end(); it != ite ; ++it)
            Tools::Delete(it->second);
        for (auto it = this->_deflatedBigChunks.begin(), ite = this->_deflatedBigChunks.end(); it != ite ; ++it)
            Tools::Delete(it->second);
    }

    void ChunkManager::Save(Tools::Database::IConnection& conn)
    {
        std::unordered_map<BigChunk::IdType, Tools::ByteArray*> bigChunks;
        BigChunk::IdType bigId;

        for (auto it = this->_chunks.begin(), ite = this->_chunks.end(); it != ite; ++it)
        {
            bigId = BigChunk::GetId(it->first);
            if (bigChunks.count(bigId) == 0)
                bigChunks[bigId] = new Tools::ByteArray();
            bigChunks[bigId]->Write(*it->second);
        }
        for (auto it = this->_deflatedChunks.begin(), ite = this->_deflatedChunks.end(); it != ite; ++it)
        {
            bigId = BigChunk::GetId(it->first);
            if (bigChunks.count(bigId) == 0)
                bigChunks[bigId] = new Tools::ByteArray();
            bigChunks[bigId]->WriteRawData(it->second->GetData(), it->second->GetSize());
        }

        std::unordered_map<BigChunk::IdType, Tools::ByteArray*> deflatedBigChunks;

        float totalInflatedSize = 0;
        float totalDeflatedSize = 0;
        Tools::Timer t;

        for (auto it = bigChunks.begin(), ite = bigChunks.end(); it != ite; ++it)
        {
            totalInflatedSize += it->second->GetSize();
            deflatedBigChunks[it->first] = this->_DeflateBigChunk(*it->second);
            totalDeflatedSize += deflatedBigChunks[it->first]->GetSize();
        }

        std::cout << "TOTAL INFLATED SIZE: " << totalInflatedSize << "\n";
        std::cout << "TOTAL DEFLATED SIZE: " << totalDeflatedSize << "\n";
        std::cout << "TOTAL SIZE LEFT: " << (totalDeflatedSize / totalInflatedSize) * 100.0f << "%\n";
        float time = (float)t.GetPreciseElapsedTime() / 1000000.0f;
        std::cout << "TOTAL TIME: " << time << "s\n";
        std::cout << "OCTETS PAR SECONDE: " << totalInflatedSize / time << "o.s-1\n";
        std::cout << "TAUX DE COMPRESSION PAR OCTETS PAR SECONDE: " << ((totalInflatedSize - totalDeflatedSize) / totalInflatedSize) * (totalInflatedSize / time) << "zlarg\n";

        auto query = conn.CreateQuery(std::string("REPLACE INTO ") + this->_map.GetName() + "_bigchunk (id, data) VALUES (?, ?)");

        for (auto it = deflatedBigChunks.begin(), ite = deflatedBigChunks.end(); it != ite; ++it)
            query->Bind(it->first).Bind((void const*)it->second->GetData(), it->second->GetSize()).ExecuteNonSelect().Reset();
        for (auto it = this->_deflatedBigChunks.begin(), ite = this->_deflatedBigChunks.end(); it != ite; ++it)
            query->Bind(it->first).Bind((void const*)it->second->GetData(), it->second->GetSize()).ExecuteNonSelect().Reset();

        for (auto it = bigChunks.begin(), ite = bigChunks.end(); it != ite; ++it)
            Tools::Delete(it->second);
        for (auto it = deflatedBigChunks.begin(), ite = deflatedBigChunks.end(); it != ite; ++it)
            Tools::Delete(it->second);

        while (this->_deflatedBigValues.size() > _maxDeflatedBig)
            this->_MoveDeflatedBigToDb(this->_deflatedBigValues.back());


        std::cout << "INFLATED CHUNKS: " << this->_chunks.size() << "\n";
        std::cout << "DEFLATED CHUNKS: " << this->_deflatedChunks.size() << "\n";
        std::cout << "DEFLATED BIG CHUNKS: " << this->_deflatedBigChunks.size() << "\n";
        std::cout << "DB BIG CHUNKS: " << this->_dbBigChunks.size() << "\n";
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
            else
            {
                auto it = this->_inflatedValues.begin();
                while (*it != id)
                    ++it;
                this->_inflatedValues.erase(it);
                this->_inflatedValues.push_front(id);
            }
        }

        Chunk* chunk = this->_chunks[id];

        this->_DeflateIfPossible(id, true);

        return chunk;
    }

    void ChunkManager::AddChunk(std::unique_ptr<Chunk> chunk)
    {
        Chunk::IdType id = chunk->id;

        // interdit d'avoir un chunk a la fois tout seul et dans des gros blocs
        if (this->_dbBigChunks.count(BigChunk::GetId(id)) == 1)
            this->_MoveDbToDeflatedBig(BigChunk::GetId(id));
        if (this->_deflatedBigChunks.count(BigChunk::GetId(id)) == 1)
            this->_MoveDeflatedBigToDeflated(BigChunk::GetId(id));

        this->_PushInflated(chunk.release());

        this->_DeflateIfPossible();
    }

    void ChunkManager::_DeflateIfPossible(Chunk::IdType noDeflateId, bool noDeflate)
    {
        //while (this->_inflatedValues.size() > _maxInflated)
        //{
        //    if (noDeflate && this->_inflatedValues.back() == noDeflateId)
        //        break;

        //    this->_MoveInflatedToDeflated(this->_inflatedValues.back());
        //}
        //while (this->_deflatedValues.size() > _maxDeflatedVal)
        //{
        //    BigChunk::IdType bigId = this->_deflatedValues.back();

        //    if (this->_inflatedChunksContainers.count(bigId) == 1)
        //        break;

        //    this->_MoveDeflatedToDeflatedBig(bigId);
        //}
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
        assert(this->_deflatedBigChunks.count(bigId) == 1);
        assert(this->_dbBigChunks.count(bigId) == 0);

        this->_PushDb(bigId);
        Tools::ByteArray* deflatedBigChunk = this->_PopDeflatedBig(bigId);
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

        this->_PushInflated(this->_InflateChunk(*array));

        Tools::Delete(array);
    }

    void ChunkManager::_MoveDeflatedBigToDeflated(BigChunk::IdType bigId)
    {
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
        assert(this->_deflatedBigChunks.count(bigId) == 0);
        assert(this->_dbBigChunks.count(bigId) == 1);

        this->_PushDeflatedBig(bigId, this->_PopDb(bigId));
    }

    Tools::ByteArray* ChunkManager::_DeflateChunk(Chunk const& chunk)
    {
        Tools::ByteArray* deflatedChunk = new Tools::ByteArray();


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

        Tools::Zlib::Worker w(_deflateLevel);
        w.Deflate(bigChunk.GetData(), bigChunk.GetSize(), (void*&)deflatedData, deflatedDataSize);

        Tools::ByteArray* deflatedBigChunk = new Tools::ByteArray();
        deflatedBigChunk->SetData(deflatedData, deflatedDataSize);

        Tools::Delete(deflatedData);


        float size = (float)deflatedDataSize / (float)bigChunk.GetSize();
        std::cout << "3Chunk Compression: " << size * 100.0f << "%\n";

        return deflatedBigChunk;
    }

    Tools::ByteArray* ChunkManager::_InflateBigChunk(Tools::ByteArray const& deflatedBigChunk)
    {
        char* inflatedData;
        unsigned int inflatedDataSize;

        Tools::Zlib::Worker w(_deflateLevel);
        w.Inflate(deflatedBigChunk.GetData(), deflatedBigChunk.GetSize(), (void*&)inflatedData, inflatedDataSize);

        Tools::ByteArray* bigChunk = new Tools::ByteArray();
        bigChunk->SetData(inflatedData, inflatedDataSize);

        Tools::Delete(inflatedData);
        return bigChunk;
    }

    void ChunkManager::_PushInflated(Chunk* chunk)
    {
        // chunk
        assert(this->_chunks.count(chunk->id) == 0);
        assert(this->_inflatedChunksContainers.count(BigChunk::GetId(chunk->id)) == 0 ||
               !this->_inflatedChunksContainers.find(BigChunk::GetId(chunk->id))->second.HasChunk(chunk->id));

        this->_chunks[chunk->id] = chunk;

        // container
        BigChunk::IdType bigId = BigChunk::GetId(chunk->id);
        if (this->_inflatedChunksContainers.count(bigId) == 0)
            this->_inflatedChunksContainers.insert(
                    std::pair<BigChunk::IdType, BigChunk>(bigId, BigChunk(bigId)));

        BigChunk& bigChunk = this->_inflatedChunksContainers.find(bigId)->second;
        bigChunk.AddChunk(chunk->id);

        // value
        this->_inflatedValues.push_front(chunk->id);
    }

    Chunk* ChunkManager::_PopInflated(Chunk::IdType id)
    {
        assert(this->_chunks.count(id) == 1);
        assert(this->_inflatedChunksContainers.count(BigChunk::GetId(id)) == 1 &&
               this->_inflatedChunksContainers.find(BigChunk::GetId(id))->second.HasChunk(id));

        // chunk
        Chunk* chunk = this->_chunks[id];
        this->_chunks.erase(id);

        // container
        BigChunk::IdType bigId = BigChunk::GetId(id);
        BigChunk& bigChunk = this->_inflatedChunksContainers.find(bigId)->second;
        bigChunk.RemoveChunk(id);
        if (bigChunk.IsEmpty())
            this->_inflatedChunksContainers.erase(bigId);

        // value
        auto it = this->_inflatedValues.begin();
        while (*it != id)
            ++it;
        this->_inflatedValues.erase(it);

        return chunk;
    }

    void ChunkManager::_PushDeflated(Chunk::IdType id, Tools::ByteArray* array)
    {
        assert(this->_deflatedChunks.count(id) == 0);
        assert(this->_deflatedChunksContainers.count(BigChunk::GetId(id)) == 0 ||
               !this->_deflatedChunksContainers.find(BigChunk::GetId(id))->second.HasChunk(id));

        // chunk
        this->_deflatedChunks[id] = array;

        // container
        bool isNu = false;
        BigChunk::IdType bigId = BigChunk::GetId(id);
        if (this->_deflatedChunksContainers.count(bigId) == 0)
        {
            this->_deflatedChunksContainers.insert(
                    std::pair<BigChunk::IdType, BigChunk>(bigId, BigChunk(bigId)));
            isNu = true;
        }
        BigChunk& bigChunk = this->_deflatedChunksContainers.find(bigId)->second;
        bigChunk.AddChunk(id);

        // value
        if (!isNu)
        {
            auto it = this->_deflatedValues.begin();
            while (*it != bigId)
                ++it;
            this->_deflatedValues.erase(it);
        }
        this->_deflatedValues.push_front(bigId);
    }

    Tools::ByteArray* ChunkManager::_PopDeflated(Chunk::IdType id)
    {
        assert(this->_deflatedChunks.count(id) == 1);
        assert(this->_deflatedChunksContainers.count(BigChunk::GetId(id)) == 1 &&
               this->_deflatedChunksContainers.find(BigChunk::GetId(id))->second.HasChunk(id));

        // chunk
        Tools::ByteArray* array = this->_deflatedChunks[id];
        this->_deflatedChunks.erase(id);

        // container
        BigChunk::IdType bigId = BigChunk::GetId(id);
        BigChunk& bigChunk = this->_deflatedChunksContainers.find(bigId)->second;
        bigChunk.RemoveChunk(id);
        if (bigChunk.IsEmpty())
        {
            this->_deflatedChunksContainers.erase(bigId);

            //value
            auto it = this->_deflatedValues.begin();
            while (*it != bigId)
                ++it;
            this->_deflatedValues.erase(it);
        }

        return array;
    }

    void ChunkManager::_PushDeflatedBig(BigChunk::IdType bigId, Tools::ByteArray* array)
    {
        assert(this->_deflatedBigChunks.count(bigId) == 0);

        // chunk
        this->_deflatedBigChunks[bigId] = array;

        // value
        this->_deflatedBigValues.push_front(bigId);
    }

    Tools::ByteArray* ChunkManager::_PopDeflatedBig(BigChunk::IdType bigId)
    {
        assert(this->_deflatedBigChunks.count(bigId) == 1);

        // chunk
        Tools::ByteArray* deflatedBigChunk = this->_deflatedBigChunks[bigId];
        this->_deflatedBigChunks.erase(bigId);

        //value
        auto it = this->_deflatedBigValues.begin();
        while (*it != bigId)
            ++it;
        this->_deflatedBigValues.erase(it);

        return deflatedBigChunk;
    }

    void ChunkManager::_PushDb(BigChunk::IdType bigId)
    {
        assert(this->_dbBigChunks.count(bigId) == 0);

        this->_dbBigChunks.insert(bigId);
    }

    Tools::ByteArray* ChunkManager::_PopDb(BigChunk::IdType bigId)
    {
        assert(this->_dbBigChunks.count(bigId) == 1);

        auto query = this->_conn.CreateQuery("SELECT data FROM " + this->_map.GetName() + "_bigchunk WHERE id = ?");
        query->Bind(bigId);
        auto row = query->Fetch();
        auto b = row->GetArray(0);

        Tools::ByteArray* deflatedBigChunk = new Tools::ByteArray();
        deflatedBigChunk->SetData(b.data(), (Uint32)b.size());

        this->_dbBigChunks.erase(bigId);

        std::cout << "POP DB (" << deflatedBigChunk->GetSize() << " octets)\n";

        return deflatedBigChunk;
    }

}}}
