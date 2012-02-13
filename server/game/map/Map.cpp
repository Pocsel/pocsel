#include "server2/game/map/Map.hpp"

#include "common/CubeType.hpp"
#include "common/Position.hpp"

#include "server2/game/map/gen/ChunkGenerator.hpp"

namespace Server { namespace Game { namespace Map {

    Map::Map(Conf const& conf, Tools::SimpleMessageQueue& gameMessageQueue) :
        _conf(conf),
        _messageQueue(gameMessageQueue),
        _spawnPosition(0)
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
    }

    void Map::Stop()
    {
        Tools::debug << "Map::Stop() -- " << this->_conf.name << "\n";
        this->_gen->Stop();
    }

    void Map::_GetChunk(Chunk::IdType id, ChunkCallback response)
    {
        auto chunk_it = this->_chunks.find(id);
        if (chunk_it == this->_chunks.end())
        {
            if (this->_chunkRequests.find(id) == this->_chunkRequests.end())
            {
                this->_chunkRequests[id].push_back(response);
                this->_gen->GetChunk(id, std::bind(&Map::HandleNewChunk, this, std::placeholders::_1));
            }
            else
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
        if (this->_spawnPosition != 0)
        {
            response(*this->_spawnPosition);
        }
        else
        {
            if (this->_spawnRequests.size() == 0)
            {
                this->_spawnRequests.push_back(response);
                this->_GetChunk(Chunk::CoordsToId(1 << 21, (1 << 19) + 30, 1 << 21),
                                std::bind(&Map::_FindSpawnPosition, this, std::placeholders::_1));
            }
            else
                this->_spawnRequests.push_back(response);
        }
    }

    void Map::_FindSpawnPosition(Chunk const& chunk)
    {
        for (int y = Common::ChunkSize - 1 ; y >= 0 ; --y)
        {
            if (chunk.GetCube(0, y, 0))
            {
                Common::CubeType const& biet = (*this->_conf.cubeTypes)[chunk.GetCube(0, y, 0) - 1];
                if (biet.solid)
                {
                    Common::Position p(chunk.coords, Tools::Vector3f(0, y, 0));
                    p += Tools::Vector3f(0.5, 2.5, 0.5);
                    this->_spawnPosition = new Common::Position(p);
                    for (auto it = this->_spawnRequests.begin(), ite = this->_spawnRequests.end(); it != ite; ++it)
                        (*it)(p);
                    this->_spawnRequests.clear();
                    return;
                }
                else
                {
                    for (unsigned int x = 0; x < Common::ChunkSize; ++x)
                    {
                        if (chunk.GetCube(x, y, 0))
                        {
                            Common::CubeType const& biet = (*this->_conf.cubeTypes)[chunk.GetCube(x, y, 0) - 1];
                            if (biet.solid)
                            {
                                Common::Position p(chunk.coords, Tools::Vector3f(x, y, 0));
                                p += Tools::Vector3f(0.5, 2.5, 0.5);
                                this->_spawnPosition = new Common::Position(p);
                                for (auto it = this->_spawnRequests.begin(), ite = this->_spawnRequests.end(); it != ite; ++it)
                                    (*it)(p);
                                this->_spawnRequests.clear();
                                return;
                            }
                        }
                    }
                    this->GetChunk(Chunk::CoordsToId(chunk.coords.x + 1, chunk.coords.y, chunk.coords.z),
                                   std::bind(&Map::_FindSpawnPosition, this, std::placeholders::_1));
                    return;
                }
            }
        }
        this->GetChunk(Chunk::CoordsToId(chunk.coords.x, chunk.coords.y - 1, chunk.coords.z),
                       std::bind(&Map::_FindSpawnPosition, this, std::placeholders::_1));
    }

}}}
