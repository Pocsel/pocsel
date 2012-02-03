#include <iostream>
#include <stdexcept>

#include "server/Map.hpp"

#include "common/Position.hpp"

namespace Server {

    Map::Map(MapConf const& conf) :
        Tools::MessageQueue<Map>(this),
        _conf(conf),
        _mapGen(_conf),
        _chunks(),
        _spawnPosition(0)
    {
    }

    Map::~Map()
    {
        auto it = this->_chunks.begin(), end = this->_chunks.end();
        for ( ; it != end ; ++it)
            delete it->second;
    }

    void Map::GetSpawnPosition(SpawnPositionCallback response)
    {
        if (this->_spawnPosition == 0)
        {
            this->_spawnRequestList.push_back(response);

            this->GetChunk(Chunk::CoordsToId(1 << 21, (1 << 19) + 30, 1 << 21),
                    std::bind(&Map::_GetSpawnPosition, this, std::placeholders::_1));
        }
        else
            response(*this->_spawnPosition);
    }

    void Map::_GetSpawnPosition(Chunk const& chunk)
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
                    for (auto it = this->_spawnRequestList.begin(), ite = this->_spawnRequestList.end(); it != ite; ++it)
                        (*it)(p);
                    this->_spawnRequestList.clear();
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
                                for (auto it = this->_spawnRequestList.begin(), ite = this->_spawnRequestList.end(); it != ite; ++it)
                                    (*it)(p);
                                this->_spawnRequestList.clear();
                                return;
                            }
                        }
                    }
                    this->GetChunk(Chunk::CoordsToId(chunk.coords.x + 1, chunk.coords.y, chunk.coords.z),
                            std::bind(&Map::_GetSpawnPosition, this, std::placeholders::_1));
                    return;
                }
            }
        }
        this->GetChunk(Chunk::CoordsToId(chunk.coords.x, chunk.coords.y - 1, chunk.coords.z),
                std::bind(&Map::_GetSpawnPosition, this, std::placeholders::_1));
    }

}
