#include "server2/game/map/Map.hpp"

#include "server2/game/map/gen/ChunkGenerator.hpp"

namespace Server { namespace Game { namespace Map {

    Map::Map(Conf const& conf) :
        Tools::SimpleMessageQueue(1),
        _conf(conf)
    {
        Tools::debug << "Map::Map() -- " << this->_conf.name << "\n";
        this->_gen = new Gen::ChunkGenerator(this->_conf);
    }

    Map::~Map()
    {
        Tools::debug << "Map::~Map() -- " << this->_conf.name << "\n";
        for (auto it = this->_chunks.begin(), ite = this->_chunks.end(); it != ite ; ++it)
            Tools::Delete(it->second);
        Tools::Delete(this->_gen);
    }

    void Map::Start()
    {
        Tools::debug << "Map::Start() -- " << this->_conf.name << "\n";
        this->_gen->Start();
        this->_Start();
    }

    void Map::Stop()
    {
        Tools::debug << "Map::Stop() -- " << this->_conf.name << "\n";
        this->_gen->Stop();
        this->_Stop();
    }

    void Map::_GetChunk(Chunk::IdType id, ChunkCallback response)
    {
        auto chunk_it = this->_chunks.find(id);
        if (chunk_it == this->_chunks.end())
        {
            if (this->_chunkRequests.find(id) == this->_chunkRequests.end())
                this->_gen->GetChunk(id, std::bind(&Map::HandleNewChunk, this, std::placeholders::_1));
            this->_chunkRequests[id].push_back(response);
        }
        else
        {
            response(*chunk_it->second);
        }
    }

    void Map::_HandleNewChunk(Chunk* chunk)
    {
        assert(this->_chunks.find(chunk->id) == this->_chunks.end() && "CHUNK ALREADY GENERATED");
        this->_chunks[chunk->id] = chunk;
        auto& requests = this->_chunkRequests[chunk->id];
        for (auto it = requests.begin(), ite = requests.end(); it != ite; ++it)
        {
            (*it)(*chunk);
        }
        requests.clear();
        this->_chunkRequests.erase(chunk->id);
    }

    void Map::_GetSpawnPosition(SpawnCallback response)
    {
        // TODO
    }

}}}
